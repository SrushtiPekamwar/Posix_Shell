// understand the getCurrentDirectory() how is it working
#include<iostream>
#include<unistd.h>
#include<sys/types.h>
#include<pwd.h>
#include<string>
#include"commands_folder/commands.h"

using namespace std;

string getUsername() {
    // get the user id and then find its username
    struct passwd *pw = getpwuid(getuid());
    if(pw) return string(pw->pw_name);
    return "unknown";
}

string getHostname() {
    char hostName[256];
    if(gethostname(hostName,sizeof(hostName))==0) return string(hostName);
    return "unknown";
}

string getCurrentDirectory(string homeDir, int flag) {
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

void initialPrompt(string homeDirectory) {
    string username = getUsername();
    string hostname = getHostname();
    string directory = getCurrentDirectory(homeDirectory,0);
    cout << "\033[34m" << "<" << username << "@" << hostname << ":" << directory << "> " << "\033[0m";
}

int main() {
    char buffer[256];
    string homeDirectory;
    if (getcwd(buffer,sizeof(buffer))) {
        homeDirectory = string(buffer);
    }
    bool running = true;

    while(running) {
        initialPrompt(homeDirectory);
        
        // read the user given cmd from the shell
        string userCommand;
        getline(cin,userCommand);
        string prevDirectory = homeDirectory;

        // ctrl+d
        if (cin.eof()) {  
            cout << endl;  
            running = false;  
            break;  
        }

        if (userCommand.empty()) {continue;}

        char* userCommandCopy = new char[userCommand.length()+1];
        strcpy(userCommandCopy,userCommand.c_str());

        // command without any ;
        const char* command = strtok(userCommandCopy,";");

        while(command) {
            while(*command==' ' || *command=='\t') {command++;}

            if(command) {
                // Shell will be terminated if the user enters exit or pressed ctrl+d
                if (strcmp(command,"exit")==0) {
                    running = false;
                    break;
                }
                else if(cin.eof()) {
                    running = false;
                    cout << endl;
                    break;
                }

                // Check if the command is "pwd"
                else if(strcmp(command,"pwd")==0) {
                    // Implement pwd here
                    string currDir = getCurrentDirectory(homeDirectory,1);
                    if(currDir!="unknown") {
                        cout << currDir << endl;
                    } else {
                        perror("pwd error");
                    }
                } 

                else if(strcmp(command,"clear")==0) {
                    cout << "\033[2J\033[3J\033[H" << flush;
                }

                else if(strncmp(command,"echo",4)==0) {
                    echoCommand(command);
                }

                else if (strncmp(command, "cd", 2) == 0 && 
                        (command[2] == '\0' || command[2] == ' ' || command[2] == '\t')) {
                    cdCommand(command, homeDirectory, prevDirectory);
                }

                else if (strncmp(command, "ls", 2) == 0 &&
                        (command[2] == '\0' || command[2] == ' ' || command[2] == '\t')) {
                    lsCommand(command, homeDirectory);
                }



                else {
                    // this is to handle invalid commands
                    cout << "Command not found: " << command << endl;
                }
            }

            command = strtok(NULL,";");  // fetching the next command into command if they are separated by ;
        }
        
        delete[] userCommandCopy;
    }


    cout << "You have exited from Srushti's terminal" << endl;
    
    return 0;
}