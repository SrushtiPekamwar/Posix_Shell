#ifndef COMMANDS_H
#define COMMANDS_H
#include <string>
#include <vector>

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
char* trimFromEnd(char *ptr);
void runExternalCommand(const char *command, string shellHomeDirectory);
void sigchldHandler(int);
string readUserInput(string &prompt);
void historyCommand(const char *command);
string getHistoryFile();
void searchCommand(const char *command);
void pinfoCommand(const char *command);
void executeWithRedirection(const char *command, string &homeDirectory, string &prevDirectory);
void parsing(char *command);
void executePipeline(const char *command, string &shellHomeDirectory);
char** autocompletion(const char *text, int start, int end);
void loadSystemCommands();
void initAutocomplete();
extern pid_t fgPid; 
void handleCtrlC(int signal);
void handleCtrlZ(int signal);

#endif
 