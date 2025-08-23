#include <string>
#include <cstdlib>
#include <pwd.h>
#include <unistd.h>

using namespace std;

string systemHomePath() {
    if(const char *home = getenv("HOME")) return string(home);
    if(passwd *pw = getpwuid(getuid())) return string(pw->pw_dir);
    return ".";
}