# AOS Assignment2 : POSIX Shell
Name : Srushti Gopal Pekamwar  
Roll number : 2025201066

## How to Compile and Run
```bash
make clean
make
./shell
```

## Project structure
2025201066_Assignment2/  
├── commands_folder/   
├── Makefile  
├── shell.cpp  
└── README.md  

## makefile
This file automates building of the shell by compiling all the .cpp files which are present in the commands_folder/ and then does all the linking of the required libraries and creates .o files

## commands.h
This file contains declaration of all the functions which are global which the shell requires.

## shell.cpp
This file is the entry point of the shell, it contains int main(). This file sets up the shell environment like setting the home directory, binding the signals, creating a history file so that the history persists even when the shell is terminated, tokenises the commands, takes the input for the user command and calls the functions so that the commands entered on this custom shell are executed.

## commands_folder
This folder contains all the .cpp files which are used for the project. Directory structure for this folder is :
commands_folder/
├── autocomplete.cpp     
├── cd.cpp               
├── commands.h           
├── echo.cpp             
├── externalCommand.cpp  
├── history.cpp          
├── home.cpp             
├── initialPrompt.cpp    
├── ls.cpp               
├── pinfo.cpp            
├── pipeline.cpp         
├── redirection.cpp      
├── search.cpp           
├── signals.cpp          
├── skipDelimiters.cpp   
└── userInput.cpp        

## autocomplete.cpp
1. loadSystemCommands - This function is used so that on pressing tab it fills with the commands which are present in the $PATH so that autocompletion feature for shell can be provided.
2. generator - This function will provide autocompletion for commands but the commands which were implemented without execvp()
3. initAutocomplete - This function will bind the TAB key for providing autocompletion functionality.
4. autocompletion - This is the function which handles autocompletion and is declared in the commands.h

## cd.cpp
1. validCdToken - This function checks whether the string starts with the cd.
2. extractArgs - This function parses the given command and then extracts the arguments so that they can be processed further.
3. expandTilde - This function will decide what will the target by checking whether it is ~ or not.
4. cdCommand - This function implements the cd command and handles all the flags like cd cd . cd .. cd -

## echo.cpp
This file contains only one function echoCommand() which will check whether it is valid echo command and if it is then it will print the output to the console.

## externalCommand.cpp
1. runExternalCommand - This function parses the command into args and then checks whether it ends with & that is background execution. Then to run the external command we need to use execvp hence it will spawn a new child process using fork(). In the child process we will setpgid(0,0) to create a new process group. If the process is running in the fg then the control is given to the child process using tcsetpgrp() and then the command is executed using execvp(). In the parent process, if the process was bg then it will print the pid and the control is returned to the shell immediately to accept more commands but if the process was fg then the parent will wait for the child process to be over using waitpid() and then it will restore control to the terminal.
2. sigchldHandler - This function will run when the bg process is done and is triggered by SIGCHLD. It used waitpid(-1,&status,WNOHANG) to reap terminated children without blocking. And once the process is done then it will print done on the shell.

## history.cpp
1. getHistoryFile - This functoin will return the path of the file which is storing the history of the commands becuase of which the history is persisiting across the shell sessions.
2. historyCommand - This function will parse the command and check whether the command is history or not, if no args are given then by defaut it will returns by default last 10 commands and if the argument is provided then it will display atmost 20 commands. This function is using GNU Readline's history_get_history_state() to retrieve commands stored in the memory.

## home.cpp
1. systemHomePath - This function returns the home directory of the system (not the shell).

## initialPrompt.cpp
1. getUsername - This function fetches the user's login name using getpwuid(getuid()).
2. getHostname - This function fetches the system's hostname using gethostname().
3. getCurrentDirectory - This function fetches the absolute path of the current working directiory usinf getcwd(). Let's say if flag=0 then this path is for prompting on the shell so the absolute path will be replaced with ~ and if flag=1 then this is used for pwd and hence it will return the full absolute path directly.
4. pwdCommand - This function will print the cwd using getCurrentDirectory() and if & is present then prompt that background proces not available for built-in processes.
5.initialPrompt - This displays the prompt for out shell like this : <srushtipekamwar@Srushtis-MacBook-Air.local:~>

## ls.cpp
1. markerOfPermissionString - This is special for macOS. It returns @ if the file has extended attributes and returns + if the file has ACLs otherwise it returns blank space.
2. filePermission - This functions handles the permissions of the file, directories. Permissions like rwx for user, others and group. This also appends the markers which will be returned from markerOfPermissionString function.
3. ownerName - This functions converts the user id to a username and falls back to numeric uid if no username is found.
4. groupName - This function will convert the group id which we will get from getgrpid to a group name and if no group name is found then it will fallback to numeric GID.
5. timeDisplay - If the file has been formatted within 6 months then it will return it in the format Month Day HH:MM and if the file is older that that then it will return in the format of Month Day Year.
6. printFiles - This prints the files just like how it is shown on the terminal along with the permissoins, time, name, etc. Regular files are coloured in green, directories as yellow and other files have default colour.
7. populateFilesArray - This function will read the entries of directory using readdir. It will skip the hidden files unless -a flag is set. It builds the full paths for these files using lstat() and then adds this entries into the filesArray.
8. parseOnlyFlagsAndPaths - This will detect the valid flags like -a, -l and then sets them in the flag. It also detects file paths and expands ~, ., .. and it returns false if the given flags are invalid.
9. lsCommand - This is the main function of this cpp file. It parses the user command into flags and paths. It no path is given then the default is taken as ., for each path if the directory is there then it opens, populates the files and prints them and if the file is refular then print the file name directly. This command supports ls, ls -a, ls -l, ls -al, ls path1 path2.

## pinfo.cpp
1. pinfoCommand - This function fetches the process details, memory usage, determines the status of the process whether it is running, sleeping, stopped, zombie and marks the fg processes with "+", it retrieves the executable path all using the pid.
Following are some structs used : 
a. proc_bsdinfo (system struct) - This struct holds the basic process information like pid, ppid, uid, gid and process status.
b. proc_taskinfo (system struct) - This struct stores process task details such as virtual memory size and runtime stats.
c. proc_pidinfo (system call) - This is used to fill the proc_bsdinfo and proc_taskinfo with process information.
d. proc_pidath (sysetem call) - This syscall retrieves the absolute path of the process's executable.
Holds basic process information like PID, PPID, UID, GID, and process status.

## pipeline.cpp
1. splitPipeline - This function will tokenise the command using pipe as the delimiter and then pushes every stage into the pipelines vector.
2. pareseStageRedirections - As this file is used for pipelines with redirections hence this function will parse the command and then extract the IO redirection operators. It stores the filenames names and then stores the operators as bool flag. It also ensures that the args array is null terminated for execvp.
3. executePipeline - This is the main function of this cpp file which handles pipelines and redirections. It will check whether the command is valid or invalid. It then calls the splitPipeline which will break the command into the pipeline stages. If there is only one single stage then it will directly execute the command. We need to create (n-1) pipes for n stages and then for each command we need to call parseStageRedirections. Thne need to fork a child process and then if redirections are present then change the stdin and stdout using dup2. Now for each stage now run the commands.

## redirection.cpp
1. restore - This function will restore the original stdin and stdout which were changed because of redirection. This function uses deup2 to copy or restore the saved file descriptors and it closes the temporary copies.
2. parsing - This parses the command string and then detects the IO redirections. Tokenisation is done using strtok_r. It also pushes tokens into tokens vector if it is not >, >>, <.
3. runSingleCommand - This function executed a single command which can either be a built-in or external and this could be done because of parsed tokens. If the command matches pwd, echo, cd, ls, pinfo, history, search then it will run the functions which were implemented by me or else it will run using execvp() command.
4. executeWithRedirection - This is the main function which is used to execute the command. First it will store the original stdin and stdout using dup. Then it will call parsing() to extract the redirection operations and token. If the input file < exists then it will open it and redirect to STDIN_FILENO and if the output file > or >> exists then it will open it and redirect to STDOUT_FILENO. Then we will call the runSingleCommand() so that the actual command is run and finaly we will restore the stdin and stdout to the original state using restore().

## search.cpp
1. searchForFile - This function will recursively search for a file/direcotry which matches with the token given. It will open the directory using opendir and then iterates over its contents using readdir and if the current entry is a directory then it will enter into that directory and recursively call searchForFile and if match not found then it will return false else returns true.
2. searchCommand - This function parses the user command and then calls the searchForFile with the given tokens, there can be multiple files or directories so it will search for all of them.

## signals.cpp
1. handleCtrlC - This function is triggered when the user presses Ctrl+C. It send SIGINT (interrupt) signaal to the fg process and this will terminate the running job immediately. If no foreground process exists then the shell ignores this signal.
2. handleCtrlZ - This function is triggerred when the use presses Ctrl+Z. This sends SIGSTP (stop) signal to the fg process and it stops the running job isntead of killing it, so it can be resumed later. If no foreground process exists then the shell ignores this signal.

## skipDelimiters.cpp
1. skipSpacesAndTabs - This function will help us to skip the tabs and the spaces present in the command.
2. trimFromEnd - This function will help us to trim the tabs and spaces from the end of the command.
3. welcomeBanner - This function prints HELLO as the welcome when the shell is started.
4. exitBanner - This function will print BYE message when terminating from the shell.

## userInput.cpp
1. upArrowKeyPressed, downArrowKeyPressed - These 2 functions bind the up and the down arrow keys to navigate through the command history. Internally they call previous-history and next-history functions from GNU readline.
2. readUserInput - This is the function which will help us to read the user input from our custom shell.
Arrow Key Handlers (upArrowKeyPressed, downArrowKeyPressed)