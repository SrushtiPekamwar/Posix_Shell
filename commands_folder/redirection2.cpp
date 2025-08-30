#include <fcntl.h>  
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
#include <sstream>
#include "commands.h"

using namespace std;

struct Redirection {
    string infile;
    string outfile;
    bool append = false;
};

// helpers
static void trim(std::string &s) {
    while (!s.empty() && isspace((unsigned char)s.back())) s.pop_back();
    size_t i = 0; while (i < s.size() && isspace((unsigned char)s[i])) ++i;
    if (i) s.erase(0, i);
}

static std::string space_ops(const std::string& s) {
    std::string out; out.reserve(s.size()*2);
    for (size_t i=0; i<s.size(); ) {
        if (s[i] == '>' && i+1<s.size() && s[i+1] == '>') { out += " >> "; i += 2; }
        else if (s[i] == '>' || s[i] == '<' || s[i] == '|') { out += ' '; out += s[i]; out += ' '; ++i; }
        else { out += s[i++]; }
    }
    return out;
}

static std::vector<std::string> tokenize_ws(const std::string& s) {
    std::vector<std::string> toks;
    char* buf = strdup(s.c_str());
    for (char* t = strtok(buf, " \t"); t; t = strtok(nullptr, " \t")) toks.emplace_back(t);
    free(buf);
    return toks;
}

static void runSingleCommand(string cmd, string &shellHomeDirectory, pid_t shellpgid, 
                             int inFd, int outFd) {
    // Save original stdin/stdout
    int savedStdin = dup(STDIN_FILENO);
    int savedStdout = dup(STDOUT_FILENO);

    // Redirect if needed
    if (inFd != STDIN_FILENO) {
        dup2(inFd, STDIN_FILENO);
        close(inFd);
    }
    if (outFd != STDOUT_FILENO) {
        dup2(outFd, STDOUT_FILENO);
        close(outFd);
    }

    // --- Builtins ---
    if (cmd.rfind("pwd", 0) == 0) {
        pwdCommand(cmd.c_str(), shellHomeDirectory);
    } else if (cmd.rfind("echo", 0) == 0) {
        echoCommand(cmd.c_str());
    } else if (cmd.rfind("cd", 0) == 0) {
        string prevDir = shellHomeDirectory;
        cdCommand(cmd.c_str(), shellHomeDirectory, prevDir);
    } else if (cmd.rfind("ls", 0) == 0) {
        lsCommand(cmd.c_str());
    } else if (cmd.rfind("pinfo", 0) == 0) {
        pinfoCommand(cmd.c_str());
    } else if (cmd.rfind("history", 0) == 0) {
        historyCommand(cmd.c_str());
    } else if (cmd.rfind("search", 0) == 0) {
        searchCommand(cmd.c_str());
    } else {
        runExternalCommand(cmd.c_str(),shellHomeDirectory);
    }

    // Restore stdin/stdout
    dup2(savedStdin, STDIN_FILENO);
    dup2(savedStdout, STDOUT_FILENO);
    close(savedStdin);
    close(savedStdout);
}

void runCommandWithRedirectionAndPipes(string fullCmd, string &shellHomeDirectory, pid_t shellpgid) {
    // Step 1: Split by "|"
    vector<string> pipeline;
    stringstream ss(fullCmd);
    string segment;
    while (getline(ss, segment, '|')) {
        pipeline.push_back(segment);
    }

    int numCmds = pipeline.size();
    int inFd = STDIN_FILENO;

    for (int i = 0; i < numCmds; i++) {
        int pipefd[2];
        int localInFd = inFd;          // start from pipeline input
        int localOutFd = STDOUT_FILENO;

        if (i < numCmds - 1) {
            pipe(pipefd);
            localOutFd = pipefd[1];
        }

        // ---- Fixed parsing with space_ops + tokenize_ws ----
        string spaced = space_ops(pipeline[i]);             // add spaces around <, >, >>
        vector<string> toks = tokenize_ws(spaced);          // split by whitespace

        string cmd;
        Redirection redir;

        for (size_t k = 0; k < toks.size(); ++k) {
            if (toks[k] == "<" && k+1 < toks.size()) {
                redir.infile = toks[++k];
            } 
            else if (toks[k] == ">" && k+1 < toks.size()) {
                redir.outfile = toks[++k];
                redir.append = false;
            } 
            else if (toks[k] == ">>" && k+1 < toks.size()) {
                redir.outfile = toks[++k];
                redir.append = true;
            } 
            else {
                if (!cmd.empty()) cmd += " ";
                cmd += toks[k];
            }
        }
        trim(cmd); trim(redir.infile); trim(redir.outfile);
        // ----------------------------------------------------

        // Input redirection
        if (!redir.infile.empty()) {
            localInFd = open(redir.infile.c_str(), O_RDONLY);
            if (localInFd < 0) {
                perror("Input file error");
                return;
            }
        }

        // Output redirection
        if (!redir.outfile.empty()) {
            int flags = O_WRONLY | O_CREAT;
            if (redir.append) flags |= O_APPEND;
            else flags |= O_TRUNC;
            localOutFd = open(redir.outfile.c_str(), flags, 0644);
            if (localOutFd < 0) {
                perror("Output file error");
                return;
            }
        }

        pid_t pid = fork();
        if (pid == 0) {
            runSingleCommand(cmd, shellHomeDirectory, shellpgid, localInFd, localOutFd);
            _exit(0);
        } else {
            if (localInFd != STDIN_FILENO) close(localInFd);
            if (localOutFd != STDOUT_FILENO) close(localOutFd);
            waitpid(pid, nullptr, 0);
            if (i < numCmds - 1) inFd = pipefd[0]; // pass next stage input
        }
    }
}