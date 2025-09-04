// <srushtipekamwar@Srushtis-MacBook-Air.local:~> open -a Terminal -W
// <srushtipekamwar@Srushtis-MacBook-Air.local:~> open -a Terminal -W &
#include <iostream>
#include <vector>
#include <cstring> 
#include <unistd.h>
#include <sys/wait.h>
#include <unordered_map>
#include "commands.h"

using namespace std;

unordered_map<pid_t,string> bgJobsList;
string shellHomeDirectory;

void sigchldHandler(int) {
    int status; 
    pid_t pid;
    // -1 means wait for any child process, status is where the exit info will be stored, wnohang means don’t block if no child has changed state 
    while((pid=waitpid(-1,&status,WNOHANG))>0) {
        auto it = bgJobsList.find(pid);
        if(it!=bgJobsList.end()) { 
            cout << endl << "[" << pid << "] + done " << it->second << endl; 
            cout << initialPrompt(shellHomeDirectory); 
            cout.flush(); 
            bgJobsList.erase(it);
        }
    }
}

void runExternalCommand(const char *command, string direct) {
    // we need to tokenise this function and then create one args array and then pass this to the execvp command
    bool background = false;
    vector<char*> args;
    shellHomeDirectory = direct;

    char *cmdCp = strdup(command);
    char *cmdCopy = cmdCp;
    if(!cmdCopy) {
        free(cmdCp);
        return;
    }
    char *checkptr = nullptr;
    char *token = strtok_r(cmdCopy," \t",&checkptr); // let's say command is open -a TextEdit & then we need to tokenise it using spaces and tabs

    string fullCommand;
    while(token) {
        if(strcmp(token,"&")==0) {
            // if it is & then we don't need to add it to the arguments array
            background = true;  // means we need to run it in the background
        }
        else {
            args.push_back(token);
            if(!fullCommand.empty()) fullCommand += " ";
            fullCommand += token;   // so that we can push it into the background and then keep listening whether the cmd has been completed
        }
        token = strtok_r(nullptr," \t",&checkptr);
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
        setpgid(0,0);
        if(!background) {tcsetpgrp(STDIN_FILENO,getpid());}
        execvp(args[0],args.data());
        perror("execvp");   // error while executing the execvp command
        free(cmdCp);
        _exit(127);
    } 
    else if(pid>0) {
        setpgid(pid,pid); // give child its separate process group
        if(background) {
           cout << "[" << pid << "] running in background" << endl;
           bgJobsList[pid] = fullCommand;
        }
        // if it is not running in the foreground then the parent process should wait for the child process to complete its execution
        else {
            // cout << "[" << pid << "] running in foreground" << endl;
            fgPid = pid;
            tcsetpgrp(STDIN_FILENO,pid);  // give child the control of the shell
            int status;
            // wait for child to exit or stop
            waitpid(pid,&status,WUNTRACED);
            // stdin_fileno is the fd of the standard input
            tcsetpgrp(STDIN_FILENO,getpgrp());   // this will give back the control to the shell
            if(WIFEXITED(status) || WIFSIGNALED(status)) {
                fgPid = -1;   // reset only if process is done
            }
        }
    }
    else {perror("fork");}

    free(cmdCp);
    return;
}