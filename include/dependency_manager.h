#ifndef DEPENDENCY_MANAGER_H
#define DEPENDENCY_MANAGER_H

#include <iostream>
#include <cstdlib>
#include <string>

#ifdef _WIN32
#include <windows.h>
#include <winreg.h>
#endif

class DependencyManager {
bool is_command_available(const std::string& command);
bool is_package_installed(const std::string& package);

#ifdef _WIN32
bool check_registry_software(const std::string& software);
#endif
};

#endif