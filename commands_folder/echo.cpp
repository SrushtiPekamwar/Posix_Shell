#include "commands.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
using namespace std;

void echoCommand(const char *command) {
    // Skip the word "echo"
    const char *ptr = command + 4; 
    ptr = skipSpacesAndTabs((char*)ptr);

    string output;
    bool inQuotes = false;
    bool lastWasSpace = false;

    while (*ptr) {
        if (*ptr == '"') {
            // Toggle quotes but don't print them
            output.push_back('"');
            inQuotes = !inQuotes;
        } 
        else if (isspace((unsigned char)*ptr)) {
            if (inQuotes) {
                // Preserve spaces inside quotes
                output.push_back(*ptr);
            } else {
                // Outside quotes: collapse to a single space
                if (!output.empty() && !lastWasSpace) {
                    output.push_back(' ');
                    lastWasSpace = true;
                }
            }
        } 
        else {
            output.push_back(*ptr);
            lastWasSpace = false;
        }
        ptr++;
    }

    cout << output << endl;
}