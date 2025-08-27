#include <iostream>
#include <readline/history.h>
#include "commands.h"

using namespace std;

void historyCommand(const char *command) {
    int n = 20; 
    const char* ptr = command + 7;
    ptr = skipSpacesAndTabs(ptr);   
    if(*ptr) {n = max(1,atoi(ptr));}

    HISTORY_STATE* currState = history_get_history_state();
    int len = 0; // default value when no count is mentioned in the history eg history 5 is not there
    if(currState) {
        len = currState->length;
    }
    if(len<=1) return;

    HIST_ENTRY** list = history_list();

    int end = len-1;  
    int start = max(0,end-n);
    for(int i=start;i<end;++i) {
        if(list[i] && list[i]->line) {
            cout << list[i]->line << endl;
        } 
    }
    cout.flush();
}