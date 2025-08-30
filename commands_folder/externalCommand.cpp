// in mac os the cmds like open -a TextEdit are short-lived; test with: sleep 3
// This is to test whether the foreground and background is working properly
// <srushtipekamwar@Srushtis-MacBook-Air.local:~> open -a Terminal -W
// <srushtipekamwar@Srushtis-MacBook-Air.local:~> open -a Terminal -W &
// [18615] running in background
#include <iostream>
#include <vector>
#include <cstring> 
#include <unistd.h>
#include <sys/wait.h>


// use the wait signal

using namespace std;

void runExternalCommand(const char *command) {
    // I need to tokenise this function and then create one args array and then pass this to the execvp command
    bool background = false;
    vector<char*> args;

    char *cmdCp = strdup(command);
    char *cmdCopy = cmdCp;
    if(!cmdCopy) {
        free(cmdCp);
        return;
    }
    char *token = strtok(cmdCopy," \t");   // let's say command is open -a TextEdit & then we need to tokenise it using spaces and tabs

    while(token) {
        if(strcmp(token,"&")==0) {
            // if it is & then we don't need to add it to the arguments array
            background = true;  // means we need to run it in the background
        }
        else {
            args.push_back(token);
        }
        token = strtok(NULL," \t");
    }
    args.push_back(NULL);  // execvp requires null terminated array

    if (args.empty()) {
        free(cmdCp);
        return;
    }

    // we need to create a child process so that we can replace its image with the external command using execvp
    pid_t pid = fork();  

    // if the pid = 0 then we will run our command in this process 
    if(pid==0) {
        execvp(args[0],args.data());
        perror("execvp");
        _exit(1);
    } 
    else if(pid>0) {
        if(background) {
           cout << "[" << pid << "] running in background" << endl;
        }
        else {
            // if it is not running in the background then the parent process should wait for the child process to complete its execution
            int status;
            waitpid(pid,&status,0);  // we need to wait only for the background process
        }
    }
    else {
        perror("fork");
    }

    free(cmdCp);
    return;
}