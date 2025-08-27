// in mac os the cmds like open -a TextEdit are short-lived; test with: sleep 3
#include <iostream>
#include <vector>
#include <cstring>     // strtok, strlen, strdup
#include <unistd.h>    // fork, execvp
#include <sys/wait.h>  // waitpid

using namespace std;

void runExternalCommand(const char* command) {
    // make a writable copy (strtok modifies the string)
    char* cmdCopy = ::strdup(command);
    if (!cmdCopy) return;

    vector<char*> argv;
    bool background = false;

    // tokenize on spaces/tabs
    for (char* tok = ::strtok(cmdCopy, " \t"); tok; tok = ::strtok(nullptr, " \t")) {
        size_t n = ::strlen(tok);

        // Case 1: a standalone "&"
        if (n == 1 && tok[0] == '&') {
            background = true;
            continue; // don't include in argv
        }

        // Case 2: token ends with '&' stuck to the last arg (e.g., "sleep&" or "sleep&")
        if (n > 0 && tok[n - 1] == '&') {
            background = true;
            tok[n - 1] = '\0';       // strip the '&'
            if (tok[0] == '\0') {
                // token became empty (it was just "&"), skip pushing it
                continue;
            }
        }

        argv.push_back(tok);
    }
    argv.push_back(nullptr);

    // Nothing to exec after stripping '&'
    if (argv.size() <= 1) { // only nullptr present
        ::free(cmdCopy);
        return;
    }

    pid_t pid = ::fork();
    if (pid < 0) {
        perror("fork");
        ::free(cmdCopy);
        return;
    }

    if (pid == 0) {
        // child
        ::execvp(argv[0], argv.data());
        // only if exec fails:
        perror("execvp");
        _exit(127);
    } else {
        // parent
        if (background) {
            // do not wait; print bg pid
            std::cout << pid << std::endl;
        } else {
            int status = 0;
            if (::waitpid(pid, &status, 0) < 0) perror("waitpid");
        }
    }

    ::free(cmdCopy);  // free the strdup'd buffer in parent
}