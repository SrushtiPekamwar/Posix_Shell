#include "commands.h"
#include <iostream>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
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
    bool quotes = false;

    while (*ptr) {
        if (*ptr == '"') {
            if(pendingSpace && outputStarted) { 
                cout << ' '; 
                pendingSpace = false; 
            }
            cout << '"';
            quotes = !quotes;
            outputStarted = true;
        } 

        else if((*ptr==' ' || *ptr=='\t')) {
            if(quotes) cout << ' ';
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