#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <cstring>
#include <iostream>
#include <vector>
#include "commands.h"

using namespace std;

bool hasInputFile = false;
bool hasOutputFile = false; 
bool append = false;
char inputFile[512] = "";
char outputFile[512] = "";
vector<string> tokens;

static void resetParsingState() {
    hasInputFile = false;
    hasOutputFile = false;
    append = false;
    inputFile[0] ='\0';
    outputFile[0] ='\0';
    tokens.clear();
}

static void restore(int stdInput, int stdOutput) {
    dup2(stdInput,STDIN_FILENO);
    dup2(stdOutput,STDOUT_FILENO);
    close(stdInput);
    close(stdOutput);
}

void parsing(char *command) {
    resetParsingState();
    char *token = strtok(command," \t");
    while(token) {
        if(strcmp(token,">")==0) {
            token = strtok(NULL," \t");
            if(token) {
                hasOutputFile = true;
                strcpy(outputFile,token);
            }
        }
        else if(strcmp(token,">>")==0) {
            token = strtok(NULL," \t");
            if(token) {
                hasOutputFile = true;
                append = true;
                strcpy(outputFile,token);
            }
        }
        else if(strcmp(token,"<")==0) {
            token = strtok(NULL," \t");
            if(token) {
                hasInputFile = true;
                strcpy(inputFile,token);
            }
        }
        else {
            tokens.push_back(token);
        }

        token = strtok(NULL," \t");
    }
}

static void runSingleCommand(string &homeDirectory, string &prevDirectory) {
    if (!tokens.empty()) {
        string str;
        for(int i=0;i<tokens.size();++i) {
            if(i) str.push_back(' ');
            str += tokens[i];
        }
        const char *finalCommand = str.c_str();

        if(tokens[0]=="pwd") pwdCommand(finalCommand,homeDirectory);
        else if(tokens[0]=="echo") echoCommand(finalCommand);
        else if(tokens[0]=="cd") cdCommand(finalCommand,homeDirectory,prevDirectory);
        else if(tokens[0]=="ls") lsCommand(finalCommand);
        else if(tokens[0]=="pinfo") pinfoCommand(finalCommand);
        else if(tokens[0]=="history") historyCommand(finalCommand);
        else if(tokens[0]=="search") searchCommand(finalCommand);
        else runExternalCommand(finalCommand,homeDirectory);
    }
}

void executeWithRedirection(const char *command, string &homeDirectory, string &prevDirectory) {
    if(!command) return;

    char *cmd = strdup(command);
    parsing(cmd);   // this will take all the arguments in the tokens

    // save the old fds and point to the new and old files
    int stdInput = dup(STDIN_FILENO);  // it just creates copy of the old fd
    int stdOutput = dup(STDOUT_FILENO);

    int inputFileFD = -1;
    int outputFileFD = -1;

    if(hasInputFile) {
        inputFileFD = open(inputFile,O_RDONLY);
        if(inputFileFD<0) {
            perror("open input"); 
            resetParsingState();
            restore(stdInput,stdOutput);
            free(cmd);
            return; 
        }
        dup2(inputFileFD,STDIN_FILENO);  // replaces the stdin_fileno with input file desc
        close(inputFileFD);
    }

    if(hasOutputFile) {
        int flags = O_WRONLY | O_CREAT;
        if(append==true) flags |= O_APPEND;
        else flags |= O_TRUNC;
        outputFileFD = open(outputFile,flags,0644);
        if(outputFileFD<0) { 
            perror("open output"); 
            resetParsingState();
            restore(stdInput,stdOutput);
            free(cmd);
            return; 
        }
        dup2(outputFileFD,STDOUT_FILENO);
        close(outputFileFD);
    }

    runSingleCommand(homeDirectory,prevDirectory);

    // need to restore the stdin and stdout
    restore(stdInput,stdOutput);
    free(cmd);
}