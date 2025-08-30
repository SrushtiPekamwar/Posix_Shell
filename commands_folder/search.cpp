#include <iostream>
#include "commands.h"
#include <dirent.h>
#include <sys/stat.h>

using namespace std;

struct fileDetails {
    string name;
    string path;
    unsigned char dtype;
    struct stat st{};
};

static bool searchForFile(char *token) {


    return true;
}

// no need to search in the hidden files
void searchCommand(const char *command) {
    char *cmdCopyOrig = strdup(command);
    char *cmdCopy = cmdCopyOrig;
    skipSpacesAndTabs(cmdCopy);
    cmdCopy+=6;   // skipping the word search
    skipSpacesAndTabs(cmdCopy);

    if(!cmdCopy) return;
    
    char *token = strtok(cmdCopy," \t");  // tokenise using the spaces and delimiters
    vector<char*> tokens;

    while(token) {
        if(strcmp(token,"&")==0) {
             cerr << "search: background execution not supported for built-in commands" << endl;
            return;
        }

        tokens.push_back(token);
        
        token = strtok(NULL," \t");
    }

    free(cmdCopyOrig);

    if (tokens.empty()) {
        cerr << "search: there should be atleast one operand in search command" << endl;
        return;
    }
}

// void lsCommand(const char *command) {
//     const char *ptr = skipSpacesAndTabs(command);
//     if(strncmp(ptr,"ls",2)==0) ptr+=2;
//     ptr = skipSpacesAndTabs(ptr);

//     string cmdStr(command);
//     if(!cmdStr.empty() && cmdStr.back()=='&') {
//         cerr << "ls: background execution not supported for built-in commands" << endl;
//         return;
//     }

//     Flags flags;
//     vector<string> filePaths;
//     if(parseOnlyFlagsAndPaths(ptr,flags,filePaths)==false) return;
//     if(filePaths.empty()) filePaths.push_back(".");
//     sort(filePaths.begin(),filePaths.end());

//     for(ssize_t i=0;i<filePaths.size();++i) {
//         string &currPath = filePaths[i];
//         struct stat st{};
//         if(lstat(currPath.c_str(),&st)!=0) {
//             perror(("ls: " + currPath).c_str());
//             continue;
//         }

//         if(S_ISDIR(st.st_mode)) {
//             DIR *currDirectory = opendir(filePaths[i].c_str());
//             if(!currDirectory) {
//                 perror(("ls: " + filePaths[i]).c_str());
//                 continue;
//             }

//             vector<fileDetails> filesArray;
//             string basePath = filePaths[i];
//             populateFilesArray(filesArray,currDirectory,basePath,flags._a);

//             if(filePaths.size()>1) cout << filePaths[i] << ":" << endl;
//             printFiles(filesArray,flags._l);
//             if((i+1)<filePaths.size()) cout << endl;
//         }
//         else {
//             cout << currPath << endl;
//         }
//     }

// }