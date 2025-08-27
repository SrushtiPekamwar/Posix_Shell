// understand the getCurrentDirectory() how is it working
#include <iostream>
#include <unistd.h>
#include <pwd.h>
#include <string>
#include "commands.h"

using namespace std;

static string getUsername() {
    // get the user id and then find its username
    struct passwd *pw = getpwuid(getuid());
    if(pw) return string(pw->pw_name);
    return "unknown";
}

static string getHostname() {
    char hostName[256];
    if(gethostname(hostName,sizeof(hostName))==0) return string(hostName);
    return "unknown";
}

static string getCurrentDirectory(string &homeDir, int flag) {
    char currentPath[256];   // absolute path of the cwd
    if(getcwd(currentPath,sizeof(currentPath))) {
        string currentPathString=currentPath;
        if(flag==0) { // means it is used for the initial prompting of the shell 
            if(currentPathString.size()>=homeDir.size() && 
            currentPathString.substr(0,homeDir.size())==homeDir) {
                return "~" + currentPathString.substr(homeDir.size());
            }
            return currentPathString;
        }
        else { // means it is for pwd 
            return currentPathString;
        }
    }
    return "unknown";
}

void pwdCommand(const char *command, string &shellHomeDirectory) {
    const char *p = command;
    p = skipSpacesAndTabs(p);
    if(strncmp(p,"pwd",3)!=0) return;
    p+=3;
    p = skipSpacesAndTabs(p);

    bool isBackground = false;
    if(*p=='&') {
        p++;
        p = skipSpacesAndTabs(p);
        if(*p=='\0') isBackground = true;
    }

    if(isBackground) {
        cerr << "pwd: background execution not supported for built-in commands" << endl;
        return;
    }

    string currDirectory = getCurrentDirectory(shellHomeDirectory,1);
    if(currDirectory!="unknown") cout << currDirectory << endl;
    else perror("pwd error");
}

string initialPrompt(string &shellHomeDirectory) {
    string username = getUsername();
    string hostname = getHostname();
    string directory = getCurrentDirectory(shellHomeDirectory,0);
    return "\033[34m" "<" + username + "@" + hostname + ":" + directory + "> " "\033[0m";
}