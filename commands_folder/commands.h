#ifndef COMMANDS_H
#define COMMANDS_H

#include <string>

using namespace std;

void echoCommand(const char* command);
void cdCommand(const char* command, string &homeDirectory, string &prevDirectory);
void lsCommand(const char* command, string &homeDirectory);
const char* skipSpacesAndTabs(const char* ptr);

#endif
