#include <iostream>
#include "commands.h"

using namespace std;

// this function will be used everywhere because we are tokenising using ; and hence to skip these spaces and \t this function
// will be used
char *skipSpacesAndTabs(char *ptr) {
    while(*ptr==' ' || *ptr=='\t') ++ptr; 
    return ptr;
}

char *trimFromEnd(char *ptr) {
    if(!ptr) return ptr;
    int len = strlen(ptr);
    while(len>=0 && (ptr[len-1]==' ' || ptr[len-1]=='\t')) {
        len--;
        ptr[len] = '\0';
    }
    return ptr;
}

void welcomeBanner() {
    cout << R"(
 _   _ _____ _     _     ___    _ _ 
| | | | ____| |   | |   / _ \  | | |
| |_| |  _| | |   | |  | | | | | | |
|  _  | |___| |___| |__| |_| | |_|_|
|_| |_|_____|_____|_____\___/  (_|_)

    )" << endl;
}

void exitBanner() {
    cout << R"(
    
     / \                       
    / _ \                      
   | / \ |                     
   ||   || _______             
   ||   || |\     \            
   ||   || ||\     \           
   ||   || || \    |           
   ||   || ||  \__/            
   ||   || ||   ||             
    \\_/ \_/ \_//              
   /   _     _   \        ____             
  /               \      | __ ) _   _  ___ 
  |    O     O    |      |  _ \| | | |/ _ \
  |   \  ___  /   |      | |_) | |_| |  __/
 /     \ \_/ /     \     |____/ \__, |\___|
/  -----  |  -----  \            |___/     
|     \__/|\__/     |                       
\       |_|_|       /                       
 \_____       _____/                        
       \     /                              
       |     |    

)" << endl;
}