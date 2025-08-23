/*
ls .
ls ..
ls ~
ls <Directory/Filename>
ls -<flags> <Directory/Filename> 
Example: <Name@UBUNTU:~> ls -al test_dir
There can be multiple directory names and multiple flags and multiple directories separately
combinations of all the flags should also work like a . .. ~
*/

// doubts of ls
// here also check if we do ls ~ then should we have to print from the root directory or the root directory of the shell

// for ls -l directory1 -a directory 2 
/*
srushtipekamwar@Srushtis-MacBook-Air ~ % ls -l Desktop -a Downloads
ls: -a: No such file or directory
Desktop:
total 40784
drwxr-xr-x@  8 srushtipekamwar  staff       256 16 Aug 14:48 2025201066_lab2
-rwxr-xr-x@  1 srushtipekamwar  staff  20382720 17 Jul 22:29 antivirus
drwxr-xr-x  10 srushtipekamwar  staff       320 20 Aug 23:33 AOS
drwxr-xr-x@ 16 srushtipekamwar  staff       512 17 Aug 18:31 CPP
drwxr-xr-x@ 15 srushtipekamwar  staff       480 19 Aug 20:47 DSAPS
drwxr-xr-x   4 srushtipekamwar  staff       128 21 Aug 13:52 PNS
-rw-r--r--@  1 srushtipekamwar  staff    495070 21 Jul 21:27 Screenshot 2025-07-21 at 9.27.24 PM.png
drwxr-xr-x  16 srushtipekamwar  staff       512 21 Aug 21:50 SSD

Downloads:
total 51224
-rw-r--r--@ 1 srushtipekamwar  staff      6064 23 Aug 10:38 generic.ovpn
-rw-r--r--@ 1 srushtipekamwar  staff    999319  4 Aug 23:13 golkonda.jpeg
-rw-r--r--@ 1 srushtipekamwar  staff   1044528 22 Aug 09:37 HTML.pdf
-rw-r--r--@ 1 srushtipekamwar  staff     30402  9 Jul 12:52 tuf_logo.png
-rw-r--r--@ 1 srushtipekamwar  staff      5691  7 Jul 18:06 tuf.png
-rw-r--r--@ 1 srushtipekamwar  staff      3546  7 Jul 18:09 tuf2.png
-rw-r--r--@ 1 srushtipekamwar  staff  20985452 23 Aug 10:37 Tunnelblick_8.0_build_6300.dmg
-rw-r--r--@ 1 srushtipekamwar  staff   2134661 22 Aug 08:42 Web.pdf
*/

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

// this is specific to mac os
#if defined(__APPLE__)
#include <sys/xattr.h>
#include <sys/acl.h>
#endif

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
    if(mode & S_ISUID) str[6] = (str[6]=='x')? 's':'S';
    if(mode & S_ISUID) str[9] = (str[9]=='x')? 't':'T';

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
                << fileSize << '\t'
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

// change the tempSt
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

// -a means show list of all the files including the hidden files
// -l means show the list of all the files in detail

// we will use this for -a and -l and any combination of al
static bool parseOnlyAFlags(const char* p, Flags &flags) {
    p = skipSpacesAndTabs(p);
    while (*p) {
        if (*p!='-') {
            return false;
        }
        p++; // - has been found and now we will move to the next
        if (!*p || *p == ' ' || *p == '\t') {
            cerr << "ls: invalid option '-'\n";
            return false;
        }
        while (*p && *p != ' ' && *p != '\t') {
            if (*p == 'a') {
                flags._a = true;
            } else if (*p == 'l') {
                flags._l = true;
            } else {
                cerr << "ls: invalid option -- '" << *p << "'\n";
                return false;
            }
            p++;
        }
        p = skipSpacesAndTabs(p);
    }
    return true;
}

void lsCommand(const char* command, string &homeDirectory) {
    const char* ptr = skipSpacesAndTabs(command);
    if(strncmp(ptr,"ls",2)==0) ptr+=2;
    ptr = skipSpacesAndTabs(ptr);

    DIR* currDirectory = opendir(".");
    if(!currDirectory) {
        cerr << "ls: cannot open current directory: " << strerror(errno) << "\n";
        return;
    }

    Flags flags;
    if (parseOnlyAFlags(ptr,flags)==false) return;

    vector<fileDetails> filesArray;
    string basePath = ".";
    populateFilesArray(filesArray,currDirectory,basePath,flags._a);
    printFiles(filesArray,flags._l);
}
