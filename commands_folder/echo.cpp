// if you want to make changes then handle the " " and if spaces are present then print as it is
// undestand this file and make changes to this
#include "commands.h"
#include <iostream>
#include <cstring>
using namespace std;

void echoCommand(const char* command) {
    const char* p = command+4;                 
    while (*p == ' ' || *p == '\t') ++p;    

    if (*p == '\0') {                       
        cout << '\n';
        return;
    }

    bool outputStarted = false;
    bool pendingSpace = false;

    while (*p) {
        if (*p == ' ' || *p == '\t') {
            pendingSpace = true;
        } else {
            if (pendingSpace && outputStarted) cout << ' ';
            cout << *p;
            outputStarted = true;
            pendingSpace = false;
        }
        ++p;
    }
    cout << '\n';
}