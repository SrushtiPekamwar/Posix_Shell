#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <string>
#include "commands_folder/commands.h"
#include <readline/history.h>
#include <readline/readline.h>

using namespace std;

int main() {    
    char buffer[1024];
    string shellHomeDirectory;
    if(getcwd(buffer,sizeof(buffer))) {
        shellHomeDirectory = string(buffer);
    }
    bool running = true;   // if this var becomes false then only we will terminate from the terminal
    string prevDirectory = shellHomeDirectory;

    // this is for the fg and bg processes
    signal(SIGTTOU,SIG_IGN);  // prevent "tty outsput suspended"
    signal(SIGTTIN,SIG_IGN);  // prevent background reads suspension

    // to handle the signals 
    // CTRL-C It should interrupt any currently running foreground job, by sending it the SIGINT signal. This
    // should have no eﬀect on the shell if there is no foreground process running
    signal(SIGINT,handleCtrlC);
    // CTRL-Z It should push any currently running foreground job into the background, and change its state
    // from running to stopped. This should have no eﬀect on the shell if there is no foreground process running
    signal(SIGTSTP,handleCtrlZ);


    // loading the previous history file 
    stifle_history(20);   // only 20 commands are stored in the history file 
    read_history(getHistoryFile().c_str());
    signal(SIGCHLD,sigchldHandler);

    // for autocompletion, when tab function is used
    loadSystemCommands();
    rl_attempted_completion_function = autocompletion;
    initAutocomplete();

    welcomeBanner();
    
    while(running) {
        string prompt = initialPrompt(shellHomeDirectory);
        string userCommand = readUserInput(prompt);

        char *userCommandCopy = new char[userCommand.length()+1];
        strcpy(userCommandCopy,userCommand.c_str());

        // command without any semi colon ;
        // we are using strtok_r because in multiple places we are calling strtok where all of them will change the global static
        // var so this will mess up everything hence we need some checkpoint pointer to resume from the same place
        char *saveptr = nullptr;
        const char *command = strtok_r(userCommandCopy,";",&saveptr);

        while(command) {
            while(*command==' ' || *command=='\t') {command++;}
            if(command) {
                // Shell will be terminated if the user enters exit or pressed ctrl+d
                if(strcmp(command,"exit")==0) {
                    running = false;
                    break;
                }

                else if(strcmp(command,"clear")==0) {
                    cout << "\033[H\033[J" << flush;   // this will clear the terminal
                }

                else if(strcmp(command,"__CTRL_D__")==0) {
                    running = false;
                    cout << endl;
                    break;
                }

                else {
                    // strchr will return the ptr to the first occurrence of |
                    if(strchr(command,'|')) {
                        executePipeline(command,shellHomeDirectory);
                    }
                    // this will be executed even when there is no redirection
                    else {
                        string cmd = command;
                        executeWithRedirection(command,shellHomeDirectory,prevDirectory);
                    }
                }
            }
            command = strtok_r(nullptr,";",&saveptr);  // fetching the next command into command if they are separated by ;
        }
        delete[] userCommandCopy;
    }

    // write to the history file before exiting 
    write_history(getHistoryFile().c_str());
    exitBanner();
    
    return 0;
}