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
├── makefile           
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
1. loadSystemCommands() - This function is used so that on pressing tab it fills with the commands which are present in the $PATH so that autocompletion feature for shell can be provided.
2. generator() - This function will provide autocompletion for commands but the commands which were implemented without execvp()
3. initAutocomplete() - This function will bind the TAB key for providing autocompletion functionality.
4. autocompletion() - This is the function which handles autocompletion and is declared in the commands.h

## cd.cpp


## echo.cpp


## externalCommand.cpp


## history.cpp


## home.cpp


## initialPrompt.cpp


## ls.cpp


## pinfo.cpp



## pipeline.cpp



## redirection.cpp


## search.cpp



## signals.cpp


## skipDelimiters.cpp


## userInput.cpp