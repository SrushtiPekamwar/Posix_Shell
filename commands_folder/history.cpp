// typedef struct _hist_state {
//     HIST_ENTRY **entries;  // Array of all history entries
//     int offset;            // Current index into the history list (cursor position)
//     int length;            // Number of entries in history
//     int size;              // Total size (capacity) of the history list
//     int flags;             // Flags (internal usage)
// } HISTORY_STATE;

// typedef struct _hist_entry {
//     char *line;   // the actual command string
//     char *data;   // application-specific data (optional, usually NULL)
// } HIST_ENTRY;

#include <iostream>
#include <readline/history.h>
#include "commands.h"

using namespace std;

// this will store the history of my shell
string getHistoryFile() {
    const char* home = getenv("HOME");
    string historyFile = string(home) + "/.SrushtiShellHistory";
    return historyFile;
}

void historyCommand(const char *command) {
    int n = 10; 
    char *cmdCp = strdup(command);
    char *ptr = cmdCp;
    ptr+=7;
    ptr = skipSpacesAndTabs(ptr);  
    if(*ptr=='&') {
        cerr << "history: background execution not supported for built-in commands" << endl;
        free(cmdCp);
        return;
    }
    if(*ptr) n = max(1,atoi(ptr));

    if(n>10) cout << "Only last 10 commands' of history are printed" << endl;

    HISTORY_STATE *currState = history_get_history_state();
    int len = 0; // default value when no count is mentioned in the history eg history 5 is not there
    if(currState) len = currState->length;
    if(len<=1) {
        free(cmdCp);
        return;
    }

    HIST_ENTRY **entries = currState->entries;

    int start = len-1-n;
    if(start<0) start = 0;
    for(int i=start;i<=len-1;++i) {
        if(entries[i] && entries[i]->line) {
            cout << entries[i]->line << endl;
        } 
    }
    cout.flush();
    free(cmdCp);
}