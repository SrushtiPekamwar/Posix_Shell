// understand the getCurrentDirectory() how is it working
// if i am doing tab and then pressing ctrl+d then it is leading to segmentation fault
// when i am entering without giving any input then also it is leading to segmentation fault

#include<iostream>
#include<unistd.h>
#include<sys/types.h>
#include<pwd.h>
#include<string>

std::string getUsername() {
    // get the user id and then find its username
    struct passwd *pw = getpwuid(getuid());
    if(pw) return std::string(pw->pw_name);
    return "unknown";
}

std::string getHostname() {
    char hostName[256];
    if(gethostname(hostName,sizeof(hostName))==0) return std::string(hostName);
    return "unknown";
}

std::string getCurrentDirectory(std::string homeDir, int flag) {
    char currentPath[256];   // absolute path of the cwd
    if(getcwd(currentPath,sizeof(currentPath))) {
        std::string currentPathString=currentPath;
        if(flag==0) { // means it is used for the initial prompting of the shell 
            if(currentPathString.size()>=homeDir.size() && 
            currentPathString.substr(0,homeDir.size())==homeDir) {
                return "~" + currentPathString.substr(homeDir.size());
            }
            return currentPathString;
        }
        else { // means it is for pwd 
            return currentPathString;
        }
    }
    return "unknown";
}

void initialPrompt(std::string homeDirectory) {
    std::string username = getUsername();
    std::string hostname = getHostname();
    std::string directory = getCurrentDirectory(homeDirectory,0);
    std::cout << "\033[34m" << "<" << username << "@" << hostname << ":" << directory << "> " << "\033[0m";
}

int main() {
    char buffer[256];
    std::string homeDirectory;
    if (getcwd(buffer,sizeof(buffer))) {
        homeDirectory = std::string(buffer);
    }
    bool running = true;

    while(running) {
        initialPrompt(homeDirectory);
        
        // read the user given cmd from the shell
        std::string userCommand;
        std::getline(std::cin,userCommand);

        if (std::cin.eof()) {
            running = false;
            continue;
        }

        char* userCommandCopy = new char[userCommand.length()+1];
        strcpy(userCommandCopy,userCommand.c_str());

        // command without any ;
        char* command = strtok(userCommandCopy,";");

        while(command) {
            while(*command==' ' || *command=='\t') {command++;}

            if(command) {
                // Shell will be terminated if the user enters exit or pressed ctrl+d
                if (strcmp(command,"exit")==0) {
                    running = false;
                    break;
                }
                else if(std::cin.eof()) {
                    running = false;
                    std::cout << std::endl;
                    break;
                }

                // Check if the command is "pwd"
                else if(strcmp(command,"pwd")==0) {
                    // Implement pwd here
                    std::string currDir = getCurrentDirectory(homeDirectory,1);
                    if(currDir!="unknown") {
                        std::cout << currDir << std::endl;
                    } else {
                        perror("pwd error");
                    }
                } 

                else if(strcmp(command,"clear")==0) {
                    std::cout << "\033[2J\033[3J\033[H" << std::flush;
                }

                else {
                    // For now, handle other commands as not found
                    std::cout << "Command not found: " << command << std::endl;
                }
            }

            // fetching the next command 
            command = strtok(NULL,";");
        }
        
        delete[] userCommandCopy;
    }


    std::cout << "You have exited from Srushti's terminal" << std::endl;
    
    return 0;
}