#include <vector>
#include <readline/readline.h>
#include <readline/history.h>
#include <dirent.h>

using namespace std;

vector<string> systemCommands;
void loadSystemCommands() {
    // /usr/bin:/bin:/usr/local/bin
    // pathEnv will be colon separated list of directories where the system looks for commands
    const char* pathEnv = getenv("PATH");
    if(!pathEnv) return;

    char *listOfDirectories = strdup(pathEnv);
    char *saveptr6;
    char *currDirectory = strtok_r(listOfDirectories,":",&saveptr6);
    while(currDirectory) {
        DIR* dirPtr = opendir(currDirectory);
        if(dirPtr) {
            struct dirent* entry;
            entry = readdir(dirPtr);
            while(entry) {
                // DT_REG is a regular file and DT_LNK is a symlink like /bin/sh so this systemCommands will just store the 
                // actual executablesw
                if(entry->d_type==DT_REG || entry->d_type==DT_LNK) systemCommands.push_back(entry->d_name);
                entry = readdir(dirPtr);
            }
            closedir(dirPtr);
        }
        currDirectory = strtok_r(nullptr,":",&saveptr6);
    }
    free(listOfDirectories);
}

static char* generator(const char* halfCommand, int state) {
    static int index;
    static vector<string> matches;

    // when it is the first time then so it needs to build the complete list but in the next subsequent times it should not build
    // it again and instead just searcg
    if(state==0) {
        index = 0;
        matches.clear();
        vector<string> inbuiltCommands = {"cd","pwd","echo","ls","pinfo","history","search","exit"};
        for(auto cmd: inbuiltCommands) {if(strncmp(cmd.c_str(),halfCommand,strlen(halfCommand))==0) matches.push_back(cmd);}
        for(auto cmd: systemCommands) {if(strncmp(cmd.c_str(),halfCommand,strlen(halfCommand))==0) matches.push_back(cmd);}
    }

    if(index<matches.size()) return strdup(matches[index++].c_str()); // strdup as we need to return the pointer
    return nullptr;
}

// command can occur anywhere because we have pipelining also so this function checks whether the position is expecting a command or 
// it is expecting a filename
static bool isCommand(const char* partialText, int start) {
    // loop backwards and search for the postion of |
    for(int i=start-1;i>=0;--i) {
        if(partialText[i]=='|') return true;
        if(partialText[i]!=' ') return false;
    }
    return true;
}

void initAutocomplete() {
    rl_bind_key('\t', rl_complete);
    rl_completion_append_character = '\0';
    rl_variable_bind("show-all-if-ambiguous", "on");
}

char **autocompletion(const char* partialText, int start, int end) {
    (void)end;  // this will stop from showing the warning that the variable is unused
    if(isCommand(rl_line_buffer,start)) return rl_completion_matches(partialText,generator);
    else return rl_completion_matches(partialText,rl_filename_completion_function);
}