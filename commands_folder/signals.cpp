// CTRL-Z It should push any currently running foreground job into the background, and change its state
// from running to terminate (T) and this could be resumed later on. 
// This should have no eﬀect on the shell if there is no foreground process running.

// CTRL-C It should interrupt any currently running foreground job, by sending it the SIGINT signal. This
// should have no eﬀect on the shell if there is no foreground process running.

#include <signal.h>
#include <iostream>
#include "commands.h"
using namespace std;

pid_t fgPid = -1; // declare the global so that this can be handled from anywhere 

void handleCtrlC(int signal) {
    // to interrupt the process
    if(fgPid>0) {
        kill(fgPid,SIGINT); 
        waitpid(fgPid,NULL,0);
        cout << endl;
    }
}

void handleCtrlZ(int signal) {
    if(fgPid>0) {
        kill(fgPid,SIGTSTP);
        cout << endl;
    }
}
