#include "commands.h"

const char* skipSpacesAndTabs(const char* ptr) {
    while(*ptr==' ' || *ptr=='\t') ++ptr; 
    return ptr;
}