#ifndef COMMANDS_H
#define COMMANDS_H
#include <string>

using namespace std; 

void welcomeBanner();
void exitBanner();
string initialPrompt(string &shellHomeDirectory);
void pwdCommand(const char *command, string &shellHomeDirectory);
void echoCommand(const char *command);
void cdCommand(const char *command, string &homeDirectory, string &prevDirectory);
void lsCommand(const char *command);
string systemHomePath();
char* skipSpacesAndTabs(char *ptr);
void runExternalCommand(const char *command);
string readUserInput(string &prompt);
void historyCommand(const char *command);
string getHistoryFile();
void searchCommand(const char *command);

#endif
