// // cd . -> stay in the same directory but still call chdir(.)
// // cd .. -> go to the previous directory 
// // If your current working directory is the directory from which your shell is invoked, then
// // on executing command "cd .." your shell should display the absolute path of the current directory from the root.
// // cd - --> goes to the previous working directory and also prints the path
// // cd --> Go to the home directory 
// // cd ~ --> go to the actual home of the system
// // cd → “go home, trust $HOME (or fallback)”
// // cd ~ → “expand ~ explicitly to $HOME and then go there”
// // handle invalid number of arguments
// add if & is present then it is not availble for background process


// need to make multiple changes in the cd

#include "commands.h"
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <unistd.h>
#include <errno.h>

// "cd" must be followed by end-of-string or at least one space/tab.
// This rejects tokens like "cd." or "cdxyz".
static bool validCdToken(const char* raw) {
    const char* p = skipSpacesAndTabs(raw);
    if (std::strncmp(p, "cd", 2) != 0) return false;
    p += 2;
    // allow end or whitespace; anything else (like '.') is invalid
    return (*p == '\0' || *p == ' ' || *p == '\t');
}

// Parse at most one argument after "cd"
static void splitOneArg(const char* raw, std::vector<std::string>& argsOut) {
    const char* p = skipSpacesAndTabs(raw);  // -> "cd..."
    p += 2;                                  // skip "cd"
    p = skipSpacesAndTabs(p);                // skip spaces after cd

    std::string curr;
    while (*p) {
        if (*p == ' ' || *p == '\t') {
            if (!curr.empty()) { argsOut.push_back(curr); curr.clear(); }
            p = skipSpacesAndTabs(p);        // collapse consecutive spaces/tabs
        } else {
            curr.push_back(*p++);
        }
    }
    if (!curr.empty()) argsOut.push_back(curr);
}

// Expand ~ to the shell's launch directory
static std::string expandTilde(const std::string& path, const std::string& shellHome) {
    if (!path.empty() && path[0]=='~') {
        if (path.size()==1) return shellHome;
        if (path[1]=='/')  return shellHome + path.substr(1);
    }
    return path;
}

void cdCommand(const char* command, std::string& shellHome, std::string& prevDir) {
    // 1) Ensure the token is truly "cd" (not "cd." etc.)
    if (!validCdToken(command)) {
        std::cerr << "Invalid arguments" << std::endl;
        return;
    }

    // 2) Parse arguments (0 or 1 allowed)
    std::vector<std::string> args;
    splitOneArg(command, args);
    if (args.size() > 1) {
        std::cerr << "Invalid arguments" << std::endl;
        return;
    }

    // 3) Resolve target path
    std::string target;
    bool isDash = false;

    if (args.empty()) {
        // cd -> go to shell home (launch directory)
        target = shellHome;
    } else if (args[0] == "-") {
        isDash = true;
        if (prevDir.empty()) {
            std::cerr << "cd: previous directory not set" << std::endl;
            return;
        }
        target = prevDir;   // we'll print new CWD after switching
    } else {
        // normal path: supports ., .., ~, ~/subdir, relative/absolute
        target = expandTilde(args[0], shellHome);
    }

    // 4) Attempt chdir, updating prevDir only on success
    char oldCwd[1024];
    if (!getcwd(oldCwd, sizeof(oldCwd))) {
        perror("cd");
        return;
    }

    if (chdir(target.c_str()) != 0) {
        perror("cd");
        return;
    }

    // success → prevDir becomes where we came from
    prevDir.assign(oldCwd);

    // cd - : print the absolute path we moved to
    if (isDash) {
        char newCwd[1024];
        if (getcwd(newCwd, sizeof(newCwd))) {
            std::cout << newCwd << std::endl;
        }
    }
}