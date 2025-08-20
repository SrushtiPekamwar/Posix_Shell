#include "commands.h"
#include<iostream>
#include<string>

void echoCommand(char* command) {
    char* args = command+4;
    while(*args==' '|| *args=='\t' || *args=='\"' || *args=='\"' || *args=='\'') {args++;}
    if(*args!='\0') {
        if(strlen(args)>0 && (args[strlen(args)-1] == '\"' || args[strlen(args)-1] == '\'')) {
            args[strlen(args)-1] = '\0';
        }
        std::cout << args << std::endl;
    } else {
        std::cout << std::endl;
    }
}