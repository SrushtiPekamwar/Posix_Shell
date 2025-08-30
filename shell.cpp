 // in cd ~ and ls ~ it should fallback to the actual home directory
// for pinfo,cd implement that if & is present the background process functionality not present for inbuilt cmds
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <string>
#include "commands_folder/commands.h"
#include <readline/history.h>

using namespace std;

int main() {
    char buffer[1024];
    string shellHomeDirectory;
    if(getcwd(buffer,sizeof(buffer))) {
        shellHomeDirectory = string(buffer);
    }
    bool running = true;   // if this var becomes false then only we will terminate from the terminal
    string prevDirectory = shellHomeDirectory;

    // loading the previous history file 
    stifle_history(20);   // only 20 commands are stored in the history file 
    read_history(getHistoryFile().c_str());

    welcomeBanner();
    
    while(running) {
        string prompt = initialPrompt(shellHomeDirectory);
        string userCommand = readUserInput(prompt);

        if(userCommand.empty()) {continue;}

        char *userCommandCopy = new char[userCommand.length()+1];
        strcpy(userCommandCopy,userCommand.c_str());

        // command without any semi colon ;
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

                else if(strcmp(command,"clear")==0) {
                    cout << "\033[2J\033[3J\033[H" << flush;   // this will clear the terminal
                
                }

                else if(strncmp(command,"pwd",3)==0) {
                    pwdCommand(command,shellHomeDirectory);
                }


                else if(strncmp(command,"echo",4)==0) {
                    echoCommand(command);
                }

                else if(strncmp(command,"cd",2)==0) {
                    cdCommand(command,shellHomeDirectory,prevDirectory);
                }

                else if(strncmp(command,"ls",2)==0) {
                    lsCommand(command);
                }

                else if(strncmp(command,"pinfo",5)==0) {
                    // pinfoCommand(command);
                }

                else if(strncmp(command,"history",7)==0) {
                    historyCommand(command);
                }

                else if(strncmp(command,"search",6)==0) {
                    searchCommand(command);
                }

                else {
                    runExternalCommand(command);
                }
            }
            command = strtok(NULL,";");  // fetching the next command into command if they are separated by ;
        }
        delete[] userCommandCopy;
    }

    // write to the history file before exiting 
    write_history(getHistoryFile().c_str());

    exitBanner();
    
    return 0;
}