#include <iostream>
#include "commands.h"
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>

using namespace std;

static bool searchForFile(const char *cwd, char *token) {
    DIR *currDirectory = opendir(cwd);
    if(!currDirectory) return false;   // this may be because you don't have permissions to access this directory

    struct dirent *files;
    files = readdir(currDirectory);
    while(files) {
        string fileName = files->d_name;
        
        // we don't need to search for this hiden files or folders 
        // . means curr directory, .. means parent directory and if the name starts with . then it means hidden file or folder
        if(fileName[0]=='.' || fileName==".." || fileName==".") {
            files = readdir(currDirectory);
            continue;
        }

        // if the filename matches with the token name then just return true
        if(strcmp(fileName.c_str(),token)==0) {
            closedir(currDirectory);
            return true;
        }

        // if it does not match then check whether it is a directory and if it is a directory then go and search in that directory
        string fullPath = string(cwd) + "/" + fileName;  // we need full path so that we can open that directory and get the stats

        struct stat st;
        if(stat(fullPath.c_str(),&st)==0 && S_ISDIR(st.st_mode)) {
            if(searchForFile(fullPath.c_str(),token)) {
                closedir(currDirectory);
                return true;
            }
        }
        files = readdir(currDirectory);
    }

    closedir(currDirectory);
    return false;
}

// no need to search in the hidden files
void searchCommand(const char *command) {
    char *cmdCopyOrig = strdup(command);
    char *cmdCopy = cmdCopyOrig;
    skipSpacesAndTabs(cmdCopy);
    cmdCopy+=strlen("search");   // skipping the word search
    skipSpacesAndTabs(cmdCopy);

    if(!cmdCopy) return;
    
    vector<string> tokens;
    char *saveptr2 = nullptr;
    char *token = strtok_r(cmdCopy," \t",&saveptr2); // tokenise using the spaces and delimiters

    while(token) {
        if(strcmp(token,"&")==0) {
            cerr << "search: background execution not supported for built-in commands" << endl;
            free(cmdCopyOrig);
            return;
        }
        tokens.push_back(string(token));
        token = strtok_r(nullptr," \t",&saveptr2);    // fetching the next token
    }

    free(cmdCopyOrig);
    if (tokens.empty()) {
        cerr << "search: there should be atleast one operand in search command" << endl;
        return;
    }

    // start searching from the cwd 
    char cwd[1024];
    getcwd(cwd,sizeof(cwd));

    // now loop about all the arguments of the tokens and then search whether the file or directory is present or not
    for(auto file: tokens) {
        if(searchForFile(cwd,(char*)file.c_str())) cout << "true" << endl;
        else cout << "false" << endl;
    }
}