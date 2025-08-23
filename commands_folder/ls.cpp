/*
Example: <Name@UBUNTU:~> ls -al test_dir
There can be multiple directory names and multiple flags and multiple directories separately
combinations of all the flags should also work like a . .. ~
when we do ls ~ then it should fallback to the actual home directory of the system
*/

// doubts of ls
// here also check if we do ls ~ then should we have to print from the root directory or the root directory of the shell
// on macos there are different things like while parsing and all so it should be according to my shell 
// ls . -a -----> -a is treated as a filename → "No such file or directory"
// ls . -a -----> -a is still treated as an option → shows hidden files in "."

// this is specific to mac os
#if defined(__APPLE__)
    #include <sys/xattr.h>
    #include <sys/acl.h>
#endif

#include "commands.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <dirent.h>
#include <cerrno>
#include <cstring>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <iomanip>
#include <ctime>
#include <cmath>

#define DEFAULT_COLOUR "\033[0m"
#define YELLOW_DIRECTORY  "\033[33m"
#define GREEN_FILES "\033[32m"

using namespace std;

struct Flags {
    bool _a = false;
    bool _l = false;
};

struct fileDetails {
    string name;
    string path;
    unsigned char dtype;
    struct stat st{};
};

static bool comp(const fileDetails &a, const fileDetails &b) {
    return a.name < b.name;
}

// this is very specific with the mac
static char markerOfPermissionString(string &path) {
    #if defined(__APPLE__)
        ssize_t extraAttribute = listxattr(path.c_str(),nullptr,0,XATTR_NOFOLLOW);
        if(extraAttribute>0) return '@';

        acl_t acl = acl_get_link_np(path.c_str(),ACL_TYPE_EXTENDED);
        if(acl) {
            acl_free(acl);
            return '+';
        }
    #endif
    return ' ';
}

static string filePermission(mode_t mode, string &path) {
    string str(10,'-');
    // the first character would be the file type
    if(S_ISDIR(mode)) str[0]='d';
    else if(S_ISLNK(mode)) str[0]='l';
    else if(S_ISCHR(mode)) str[0]='c';
    else if(S_ISBLK(mode)) str[0]='b';
    else if(S_ISFIFO(mode)) str[0]='p';
    else if(S_ISSOCK(mode)) str[0]='s';
    else str[0]='-';

    // now we will append the user, groups and the others permisssion
    str[1]=(mode & S_IRUSR)? 'r':'-';
    str[2]=(mode & S_IWUSR)? 'w':'-';
    str[3]=(mode & S_IXUSR)? 'x':'-';

    str[4]=(mode & S_IRGRP)? 'r':'-';
    str[5]=(mode & S_IWGRP)? 'w':'-';
    str[6]=(mode & S_IXGRP)? 'x':'-';

    str[7]=(mode & S_IROTH)? 'r':'-';
    str[8]=(mode & S_IWOTH)? 'w':'-';
    str[9]=(mode & S_IXOTH)? 'x':'-';

    // the last character is the special bit 
    if(mode & S_ISUID) str[3] = (str[3]=='x')? 's':'S';
    if(mode & S_ISGID) str[6] = (str[6]=='x')? 's':'S';
    if(mode & S_ISVTX) str[9] = (str[9]=='x')? 't':'T';

    string finalStr = str;
    finalStr.push_back(markerOfPermissionString(path));
    return finalStr;
}

static string ownerName(uid_t uid) {
    if(passwd *pw = getpwuid(uid)) {
        if(pw->pw_name) return pw->pw_name;
    }
    return to_string(uid);
}

static string groupName(gid_t gid) {
    if(group *gr = getgrgid(gid)) {
        if(gr->gr_name) return gr->gr_name;
    }
    return to_string(gid);
}

// if the file has been modified in the last 6 months then formatting would be Day dd hh:mm
// if it is older than that then day dd yyyy
static string timeDisplay(time_t modifiedTime) {
    char buffer[64];
    tm tm{};
    localtime_r(&modifiedTime,&tm);
    time_t currTime = time(nullptr);
    double difference = difftime(currTime,modifiedTime);
    double sixMonthsInSec = 60*60*24*182.5;

    if (difference>sixMonthsInSec || modifiedTime>currTime) {
        strftime(buffer,sizeof(buffer),"%b %e  %Y",&tm);
    } else {
        strftime(buffer,sizeof(buffer),"%b %e %H:%M",&tm);
    }
    return string(buffer);
}

static void printFiles(vector<fileDetails> &filesArray, bool _l) {
    sort(filesArray.begin(),filesArray.end(),comp);
    if(_l) {
        long long total = 0;
        for(auto file: filesArray) {
            total += file.st.st_blocks;
        }
        cout << "total " << total << endl;
        for (auto file : filesArray) {
            string permissions = filePermission(file.st.st_mode,file.path);
            unsigned long long nlink = static_cast<unsigned long long>(file.st.st_nlink);
            unsigned long long fileSize = static_cast<unsigned long long>(file.st.st_size);
            string time = timeDisplay(file.st.st_mtime);

            string color = DEFAULT_COLOUR;
            if(file.dtype==DT_DIR) color = YELLOW_DIRECTORY;
            else if(file.dtype==DT_REG) color = GREEN_FILES;

            cout << color
                << permissions << '\t'
                << nlink << '\t'
                << ownerName(file.st.st_uid) << '\t' << '\t'
                << groupName(file.st.st_gid) << '\t'
                << fileSize << '\t' << '\t' << '\t'
                << time << '\t'
                << file.name 
                << DEFAULT_COLOUR
                << endl;
        }
    }
    else {
        for(auto file: filesArray) {
            string color = DEFAULT_COLOUR;
            if(file.dtype==DT_DIR) color = YELLOW_DIRECTORY;
            else if(file.dtype==DT_REG) color = GREEN_FILES;

            cout << color << file.name << DEFAULT_COLOUR << endl;
        }
    }
}

static void populateFilesArray(vector<fileDetails> &filesArray, DIR* currDirectory, string &basePath, bool _a) {
    // readdir reads entry from the open directory stream
    for(dirent* files;(files=readdir(currDirectory));) {
        fileDetails fDetails;
        fDetails.name = files->d_name;
        fDetails.dtype = files->d_type; 
        if(!_a && !fDetails.name.empty() && fDetails.name[0]=='.') continue; // skipping the hidden files unless it is -a flag

        // building the path for the file
        if (basePath.empty() || basePath == ".") fDetails.path = fDetails.name;
        else fDetails.path = basePath + "/" + fDetails.name;


        if(lstat(fDetails.path.c_str(),&fDetails.st)!=0) {
            perror(("ls: cannot access " + fDetails.name).c_str());
            memset(&fDetails.st,0,sizeof(struct stat));
        }
        filesArray.emplace_back(std::move(fDetails));
    }
    closedir(currDirectory);
}

static string interpretingTokens(string& token) {
    string shellHomePath = systemHomePath();
    // when only ~ is present 
    if(token=="~") {
        return shellHomePath;
    }
    // when ~ and / are present 
    else if (token.size() > 1 && token[0] == '~' && token[1] == '/') {
        return string(shellHomePath) + token.substr(1);
    }
    // when tokens like . .. are present 
    return token;
}

// -a means show list of all the files including the hidden files
// -l means show the list of all the files in detail

// we will use this for -a and -l and any combination of al and also to parse flags like . .. ~
static bool parseOnlyFlagsAndPaths(const char* ptr, Flags &flags, vector<string> &filePaths) {
    ptr = skipSpacesAndTabs(ptr);
    bool seenTheOperand = false;
    while (*ptr) {
        // handling args like -a -l
        if(seenTheOperand==false && *ptr=='-') {
            ptr++; // - has been found and now we will move to the next
            if (!*ptr || *ptr==' ' || *ptr=='\t') {
                cerr << "ls: invalid option '-'\n";
                return false;
            }
            while (*ptr && *ptr!=' ' && *ptr!='\t') {
                if (*ptr=='a') {flags._a = true;}
                else if (*ptr=='l') {flags._l = true;}
                else {
                    cerr << "ls: invalid option -- '" << *ptr << endl;
                    return false;
                }
                ptr++;
            }
        }
        // there must be flags like . .. ~
        else {
            string token;
            while(*ptr && *ptr!=' ' && *ptr!='\t') token.push_back(*ptr++);
            if(!token.empty()) {
                filePaths.push_back(interpretingTokens(token));
                seenTheOperand = true;
            }
        }
        ptr = skipSpacesAndTabs(ptr);
    }
    return true;
}

void lsCommand(const char* command) {
    const char* ptr = skipSpacesAndTabs(command);
    if(strncmp(ptr,"ls",2)==0) ptr+=2;
    ptr = skipSpacesAndTabs(ptr);

    Flags flags;
    vector<string> filePaths;
    if(parseOnlyFlagsAndPaths(ptr,flags,filePaths)==false) return;
    if(filePaths.empty()) filePaths.push_back(".");
    sort(filePaths.begin(),filePaths.end());

    for(ssize_t i=0;i<filePaths.size();++i) {
        string &currPath = filePaths[i];
        struct stat st{};
        if(lstat(currPath.c_str(),&st)!=0) {
            perror(("ls: " + currPath).c_str());
            continue;
        }

        if(S_ISDIR(st.st_mode)) {
            DIR* currDirectory = opendir(filePaths[i].c_str());
            if(!currDirectory) {
                perror(("ls: " + filePaths[i]).c_str());
                continue;
            }

            vector<fileDetails> filesArray;
            string basePath = filePaths[i];
            populateFilesArray(filesArray,currDirectory,basePath,flags._a);

            if(filePaths.size()>1) cout << filePaths[i] << ":" << endl;
            printFiles(filesArray,flags._l);
            if((i+1)<filePaths.size()) cout << endl;
        }
        else {
            cout << currPath << endl;
        }
    }

}
