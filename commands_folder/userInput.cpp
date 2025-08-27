#include <iostream>
#include <string>
#include <readline/readline.h>
#include <readline/history.h>
#include "commands.h"

using namespace std;

static bool arrowBinding = true;

static int downArrowKeyPressed(int count, int key) {
    rl_named_function("next-history")(count,key);
    rl_redisplay();
    cout.flush();
    return 0;
}

static int upArrowKeyPressed(int count, int key) {
    rl_named_function("previous-history")(count,key);
    rl_redisplay();
    cout.flush();
    return 0;
}

static void bindToArrowKeys() {
    arrowBinding = false;
    if(arrowBinding) return;
    rl_bind_keyseq(const_cast<char*>("\e[A"),upArrowKeyPressed); 
    rl_bind_keyseq(const_cast<char*>("\e[B"),downArrowKeyPressed); 
    arrowBinding = true;
}

string readUserInput(string &prompt) {
    bindToArrowKeys();
    cout.flush();

    char *line = readline(prompt.c_str()); // this will read the initial prompt and also read the input from the command line 
    if(!line) return ""; // if ctrl+d is pressed then nullptr is passed

    string input(line);
    if(!input.empty()) {
        add_history(line);
    }
    free(line);
    return input;
}