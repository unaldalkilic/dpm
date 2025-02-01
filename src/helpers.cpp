#include "../include/helpers.h"

std::string get_cwd() {
    char cwd[1024];
    #ifdef _WIN32
        _getcwd(cwd, sizeof(cwd));
    #else
        getcwd(cwd, sizeof(cwd));
    #endif
    
    return std::string(cwd);
}