// executeWithRedirection(stages[0],shellHomeDirectory,shellHomeDirectory);
// for every stage redirection parsing change the code and do it according to the redirection.cpp
// add this if required

#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include <vector>
#include <string>
#include <iostream>
#include "commands.h"
#include <fcntl.h>

using namespace std;

// tokenise using |
static void splitPipeline(char* cmd, vector<char*> &pipelines) {
    pipelines.clear();
    char *saveptr3 = nullptr;
    char *token = strtok_r(cmd,"|",&saveptr3);
    while(token) {
        token = skipSpacesAndTabs(token);
        token = trimFromEnd(token);
        if(*token) pipelines.push_back(token);
        token = strtok_r(nullptr,"|",&saveptr3);
    }
}

static void stripQuotes(char* str) {
    int n = strlen(str);
    if(n>=2 && str[0]=='"' && str[n-1]=='"') {
        memmove(str,str+1,n-2);
        str[n-2] = '\0';
    }
}

static void parseStageRedirections(char *command, vector<char*> &tokens, bool &hasInputFile,  char inputFile[512], 
                                   bool &hasOutputFile, char outputFile[512], bool &append) {
    tokens.clear();
    hasInputFile = false;
    hasOutputFile = false;
    append = false;
    inputFile[0] = '\0';
    outputFile[0] = '\0';

    char *saveptr5 = nullptr;
    char *token = strtok_r(command," \t",&saveptr5);
    while(token) {
        if(strcmp(token,">")==0) {
            token = strtok_r(nullptr," \t",&saveptr5);
            if(token) {
                hasOutputFile = true;
                strcpy(outputFile,token);
            }
        }
        else if(strcmp(token,">>")==0) {
            token = strtok_r(nullptr," \t",&saveptr5);
            if(token) {
                hasOutputFile = true;
                append = true;
                strcpy(outputFile,token);
            }
        }
        else if(strcmp(token,"<")==0) {
            token = strtok_r(nullptr," \t",&saveptr5);
            if(token) {
                hasInputFile = true;
                strcpy(inputFile,token);
            }
        }
        else {
            tokens.push_back(token);
        }
        token = strtok_r(nullptr," \t",&saveptr5);
    }
}

void executePipeline(const char* command, string &shellHomeDirectory) {
    if(!command) return;

    string str(command);
    auto first = str.find_first_not_of(" \t");
    if(first==string::npos) return;

    // npos means no position found
    auto last = str.find_last_not_of(" \t");
    if(str[first]=='|' || str[last]=='|' || str.find("||")!=string::npos) {
        cerr << "shell: syntax error near unexpected token '|'\n";
        return;
    }

    char* line = strdup(command); 
    if(!line) { 
        perror("strdup"); 
        return; 
    }

    vector<char*> stages;
    splitPipeline(line,stages);
    if(stages.empty()) { 
        free(line); 
        return; 
    }

    int n = (int)stages.size();
    // if there is only single stage then just execute it
    if(n==1) {
        // executeWithRedirection(stages[0],shellHomeDirectory,shellHomeDirectory);
        runExternalCommand(stages[0],shellHomeDirectory);
        free(line);
        return;
    }

    // for n stages we require (n-1) pipes
    vector<int> pipes(2*(n-1),-1);
    for(int i=0;i<n-1;++i) {
        // means we are asking the kernel to create a new pipe, telling it to store the two fd in pipes[2*i] and pipes[2*i+1]
        if(pipe(&pipes[2*i])<0) {
            perror("pipe");
            free(line);
            return;
        }
    }

    // Launching of each stage
    for(int i=0;i<n;++i) {
        char* singleStage = strdup(stages[i]);
        if(!singleStage) { 
            perror("strdup"); 
            break; 
        }

        vector<char*> args;
        // this will convert each stage into different args so that it can run the single command
        bool hasInputFile = false;
        bool hasOutputFile = false;
        bool append = false;
        char inputFile[512];
        char outputFile[512];
        parseStageRedirections(singleStage,args,hasInputFile,inputFile,hasOutputFile,outputFile,append);

        for(int i=0;args[i];++i) stripQuotes(args[i]);

        pid_t pid = fork();
        if(pid==0) {
            // for the child we will change the std in and std output
            // if it is not the first pipe then only we can have read end of the prev pipe
            if(hasInputFile) {
                int fd = open(inputFile,O_RDONLY);
                if(fd<0) { 
                    perror("open <"); 
                    _exit(127); 
                }
                if(dup2(fd,STDIN_FILENO)<0) { 
                    perror("dup2 <"); 
                    _exit(127); 
                }
                close(fd);
            } 
            else if(i>0) {
                int inputFd = pipes[2*(i-1)];     // we need to use the read end of the previous pipe
                if(dup2(inputFd,STDIN_FILENO)<0) { 
                    perror("dup2 stdin"); 
                    _exit(127);   // 127 is used by the shell when the command is not found
                }
            }

            if(hasOutputFile) {
                int flags = O_WRONLY | O_CREAT;
                if(append==true) flags |= O_APPEND;
                else flags |= O_TRUNC;
                int fd = open(outputFile,flags,0644);
                if(fd<0) { 
                    perror("open >"); 
                    _exit(127); 
                }
                if(dup2(fd,STDOUT_FILENO)<0) { 
                    perror("dup2 >");
                    _exit(127); 
                }
                close(fd);
            } 
            // if its not the last stage then only we will have the read end
            else if(i<n-1) {
                int outputFd = pipes[2*i+1];    // we need to use the write end of the same pipe
                if(dup2(outputFd, STDOUT_FILENO)<0) { 
                    perror("dup2 stdout"); 
                    _exit(127); 
                }
            }

            // Close all pipe fds in the child
            for(int k=0;k<pipes.size();++k) close(pipes[k]);

            // Exec external command
            if(!args.empty() && args[0]) {
                const char* finalCommand = stages[i];
                string commandName = args[0];

                // just for debugging
                // fprintf(stderr, "[stage %d] in<%s> out<%s%s>\n",
                // i,
                // hasInputFile ? inputFile : "-",
                // hasOutputFile ? outputFile : "-",
                // hasOutputFile ? (append ? " (append)" : " (truncate)") : "");

                if(commandName=="pwd") {
                    pwdCommand(finalCommand,shellHomeDirectory);
                    _exit(0);  // terminate child cleanly
                }
                else if(commandName=="echo") {  
                    echoCommand(finalCommand);
                    _exit(0);
                }
                else if(commandName=="ls") {
                    lsCommand(finalCommand);
                    _exit(0);
                }
                else if(commandName=="pinfo") {
                    pinfoCommand(finalCommand);
                    _exit(0);
                }
                else if(commandName=="history") {
                    historyCommand(finalCommand);
                    _exit(0);
                }
                else if(commandName=="search") {
                    searchCommand(finalCommand);
                    _exit(0);
                }
                else {
                    execvp(args[0],args.data()); 
                    perror("execvp");
                    _exit(127);
                }
            }

            perror("execvp");
            _exit(127);
        } 
        else if(pid<0) {
            perror("fork");
            free(singleStage);
            for(int k=0;k<pipes.size();++k) {
                if(pipes[k]>=0) close(pipes[k]);
            }
            int status;
            while(wait(&status)>0);  // parent will wait till wait returns -1 means no more children are present
            free(line);
            return;
        }

        free(singleStage);
        // Parent continues to next stage
    }

    // parent will close all pipe fds and wait for children
    for(int k=0;k<pipes.size();++k) {
        if(pipes[k]>=0) close(pipes[k]);
    }
    int status;
    while(wait(&status)>0);  // parent will wait till wait returns -1 means no more children are present

    free(line);
}


/*
working of pipe 
A | B | C
You need 2 fd for each pipe one to read and one to write: 
Pipe 0: connect A → B
A writes to pipes[1]
B reads from pipes[0]
Pipe 1: connect B → C
B writes to pipes[3]
C reads from pipes[2]

pipes[2*i] is the read end of pipe i
pipes[2*i+1] is the write end of pipe i
*/