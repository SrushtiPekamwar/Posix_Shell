// open -a Terminal -W
// open -a Terminal -W &
#include <iostream>
#include <cstring>
#include "commands.h"
#include <unistd.h>
#include <libproc.h>
#include <sys/proc_info.h>
#include <sys/types.h>

using namespace std;

extern pid_t fgPid;

void pinfoCommand(const char* command) {
    char *cmdCp = strdup(command);
    char *ptr = cmdCp;

    ptr = skipSpacesAndTabs(ptr);
    ptr += 5; // skip the word pinfo
    ptr = skipSpacesAndTabs(ptr);

    if(*ptr=='&') {
        cerr << "pinfo: background execution not supported for built-in commands" << endl;
        free(cmdCp);
        return;
    }

    pid_t pid;
    if(*ptr=='\0') {
        // means no args are present so fetch the pid of the working shell
        pid = getpid();
    }
    else {
        // fetch the pid
        pid = atoi(ptr);
    }


    // this struct contains information about a process at BSD level like process ID, parent PID, user IDs, status, virtual memory size, command name
    // this is just an empty struct which will be filled with information using proc_pidfdinfo
    struct proc_bsdinfo proc;
    // proc_pidinfo will give detailes information about process with pid
    if(proc_pidinfo(pid,PROC_PIDTBSDINFO,0,&proc,sizeof(proc))<=0) { // if >0 means it is returning number of bytes written in buffer
        cerr << "pinfo " << pid << ": either it is an invalid pid or it has completed its execution" << endl;
        free(cmdCp);
        return;
    }

    // this will contain the memory related infor
    struct proc_taskinfo taskinfo;
    if(proc_pidinfo(pid,PROC_PIDTASKINFO,0,&taskinfo,sizeof(taskinfo))<=0) {
        cerr << "pinfo: could not fetch task info for pid" << pid << endl;
        free(cmdCp);
        return;
    }
    unsigned long long virtualMemory = taskinfo.pti_virtual_size;   // virtual memory required in bytes

    // mapping to R,S,T,Z
    string status = "?";  // when the status is unknown
    int processStatus = proc.pbi_status;
    if(processStatus==2) status="R";  // SRUN
    else if(processStatus==3) status="S";  // SSLEEP
    else if(processStatus==4) status="T";  // SSTOP
    else if(processStatus==5) status="Z";  // SZOMBa

    // We need to add "+" if the process is running in the foreground
    // processes are grouped in a process group
    // first we will get the group id of the our shell as it is working in the fg and then get the group id of the procedure and 
    // if both the group ids are same then we can conclude that the process is runnning in the foreground
    // cout << "pid, fgpid : " << pid << " " << fgPid << endl;
    // Fallback: check with terminal process group (for other processes)
    pid_t groupidOfShell = tcgetpgrp(STDIN_FILENO);
    pid_t groupidOfcommand = getpgid(pid);
    if(groupidOfShell==groupidOfcommand) {
        status+="+";
    }

    char executablePath[PROC_PIDPATHINFO_MAXSIZE];
    if(proc_pidpath(pid,executablePath,sizeof(executablePath))<=0) {
        strcpy(executablePath,"Unknown");
    }

    cout << "pid : " << pid << endl;
    cout << "Process Status : " << status << endl;
    cout << "Virutal Memory Usage (bytes) : " << virtualMemory << endl;
    cout << "Executable Path : " << executablePath << endl;
 
    free(cmdCp);
}


// SRUN → 'R' : means the process is running or runnable so its either actively executing on the cpu or ready to run as soon as the 
// scheduler gives it CPU time.
// SSLEEP → 'S' : means the process is sleeping or waiting so its not using the CPU, but it's waiting for some event like IO or disk read.
// SSTOP → 'T' : means the process is stopped and it usually happens when you press ctrl+z in the terminal and send a SIGTSTP
// SZOMB → 'Z' : means the process is a zombie and it has already finished its execution but its parent hasn't called waitpid() to 
// clean up its entry yet.