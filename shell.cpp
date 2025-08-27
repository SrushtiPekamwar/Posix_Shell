// in the search no need to consider the hidden files 
// in cd ~ and ls ~ it should fallback to the actual home directory
// for pinfo,cd implement that if & is present the background process functionality not present for inbuilt cmds
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <string>
#include "commands_folder/commands.h"

using namespace std;

int main() {
    char buffer[256];
    string shellHomeDirectory;
    if (getcwd(buffer,sizeof(buffer))) {
        shellHomeDirectory = string(buffer);
    }
    bool running = true;
    string prevDirectory = shellHomeDirectory;
    
    while(running) {
        string prompt = initialPrompt(shellHomeDirectory);
        string userCommand = readUserInput(prompt);

        if (userCommand.empty()) {continue;}

        char *userCommandCopy = new char[userCommand.length()+1];
        strcpy(userCommandCopy,userCommand.c_str());

        // command without any ;
        const char *command = strtok(userCommandCopy,";");

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

                else if(strncmp(command,"pwd",3)==0 &&
                        (command[3]=='\0' || command[3]==' ' || command[3]=='\t' || command[3]=='&')) {
                    pwdCommand(command, shellHomeDirectory);
                }

                else if(strcmp(command,"clear")==0) {
                    cout << "\033[2J\033[3J\033[H" << flush;
                
                }

                else if(strncmp(command,"echo",4)==0) {
                    echoCommand(command);
                }

                else if(strncmp(command,"cd",2)==0 && 
                       (command[2]=='\0' || command[2]==' ' || command[2]=='\t')) {
                    cdCommand(command, shellHomeDirectory, prevDirectory);
                }

                else if(strncmp(command,"ls",2)==0 &&
                       (command[2]=='\0' || command[2]==' ' || command[2]=='\t')) {
                    lsCommand(command);
                }

                else if(strncmp(command,"history",7)==0 &&
                        (command[7]=='\0' || command[7]==' ' || command[7]=='\t')) {
                    historyCommand(command);
                }

                // this is for pinfo, implement it later 
                else if(strncmp(command,"pinfo",5)==0 &&
                        (command[5]=='\0' || command[5]==' ' || command[5] =='\t')) {
                    // pinfoCommand(command);
                }

                else {
                    runExternalCommand(command);
                }
            }
            command = strtok(NULL,";");  // fetching the next command into command if they are separated by ;
        }
        delete[] userCommandCopy;
    }
    cout << "You have exited from Srushti's terminal" << endl;
    
    return 0;
}