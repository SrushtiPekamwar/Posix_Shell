#include "commands.h"
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <unistd.h>
#include <errno.h>

using namespace std;

static bool validCdToken(char *command) {
    char* ptr = skipSpacesAndTabs(command);
    if(strncmp(ptr,"cd",2)!=0) return false;
    ptr += 2; // skip cd

    // allow end or whitespace; anything else (like '.') is invalid
    return (*ptr=='\0' || *ptr==' ' || *ptr=='\t');
}

static void extractArgs(char *command, vector<string> &args) {
    char *ptr = skipSpacesAndTabs(command); 
    ptr += 2;        
    ptr = skipSpacesAndTabs(ptr);                                

    string curr;
    while(*ptr) {
        if(*ptr==' ' || *ptr=='\t') {
            if(!curr.empty()) {
                args.push_back(curr);
                curr.clear();
            }
            ptr = skipSpacesAndTabs(ptr);
        }
        else {
            curr.push_back(*ptr);
            ptr++;
        }
    }
    if(!curr.empty()) args.push_back(curr);
}

// when it is ~ then we need to go to the system's home directory
static string expandTilde(string &token) {
    string shellHomePath = systemHomePath();
    if(!token.empty() && token[0]=='~') {
        if(token.size()==1) return shellHomePath;
        if(token[1]=='/') return shellHomePath + token.substr(1);
    }
    return token;
}

void cdCommand(const char *command, string &shellHome, string &prevDir) {
    char *cmdCp = strdup(command);
    char *ptr = cmdCp;

    if(!validCdToken(ptr)) {
        cerr << "Invalid arguments for cd command" << endl;
        free(cmdCp);
        return;
    }

    string cmdStr(ptr);
    if(!cmdStr.empty() && cmdStr.back()=='&') {
        cerr << "cd: background execution not supported for built-in commands" << endl;
        return;
    }

    // in case of cd there can be atmost 1 argument
    vector<string> args;  // we will store the arguments of cd
    extractArgs(ptr,args);

    if(args.size()>1) {
        cerr << "Invalid arguments" << endl;
        free(cmdCp);
        return;
    }

    // where to go 
    string target;
    bool isDash = false;

    // checking what flags are present in the cd
    if(args.empty()) {
        // where it is only cd then you should go to shell's home directory 
        target = shellHome;
    } 
    else if(args[0]=="-") {
        isDash = true;
        target = prevDir;
    } 
    else {
        target = expandTilde(args[0]);
    }

    // for - we would require the this directory which will become the prev working directory and hence we need to 
    char oldCwd[1024];
    if(!getcwd(oldCwd,sizeof(oldCwd))) {
        perror("cd");
        free(cmdCp);
        return;
    }

    // chdir will handle the cases for . and .. and things like cd directory_name
    if(chdir(target.c_str())!=0) {
        perror("cd");
        free(cmdCp);
        return;
    }

    prevDir = oldCwd;

    // cd - then we need to print the absolute path for the prev working directory and then print its path
    if(isDash) {
        char newCwd[1024];
        if(getcwd(newCwd,sizeof(newCwd))) {
            cout << newCwd << endl;
        }
    }
    free(cmdCp);
}