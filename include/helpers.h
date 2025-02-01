#ifndef HELPERS_H
#define HELPERS_H

#include <string>
#ifdef _WIN32
#include <dirent.h>
#else
#include <unistd.h>
#endif

std::string get_cwd();

#endif