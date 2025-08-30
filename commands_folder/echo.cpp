#include "commands.h"
#include <iostream>
#include <cstring>
using namespace std;

void echoCommand(const char* command) {
    char *cmdCopy = strdup(command);
    char *ptr = cmdCopy;
    ptr+=4;                 
    ptr = skipSpacesAndTabs(ptr);   
    if(*ptr=='&') {
        cerr << "echo: background execution not supported for built-in commands" << endl;
        free(cmdCopy);
        return;
    }

    // if nothing is present after echo or only spaces are there then just print new line
    if(*ptr=='\0') {                       
        cout << endl;
        free(cmdCopy);
        return;
    }

    bool outputStarted = false;
    bool pendingSpace = false;

    while (*ptr) {
        if(*ptr==' ' || *ptr=='\t') {
            pendingSpace = true;
        } 
        else {
            if(pendingSpace && outputStarted) {cout << ' ';}  // this will ensure that all the whitespaces will be replaced with single whitespace
            cout << *ptr;
            outputStarted = true;
            pendingSpace = false;
        }
        ++ptr;
    }
    cout << endl;
    free(cmdCopy);
}