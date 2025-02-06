#include "../include/dependency_manager.hpp"

bool DependencyManager::is_command_available(const std::string& command) {
#ifdef _WIN32
    std::string cmd = "where " + command + " >nul 2>nul";
#else
    std::string cmd = "which " + command + " > /dev/null 2>&1";
#endif
    return (std::system(cmd.c_str()) == 0);
}

#ifdef _WIN32
bool DependencyManager::check_registry_software(const std::string& software) {
    HKEY hKey;
    std::string regPath = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall";
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, regPath.c_str(), 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        char name[256];
        DWORD nameSize = sizeof(name);
        for (DWORD i = 0; RegEnumKeyEx(hKey, i, name, &nameSize, nullptr, nullptr, nullptr, nullptr) == ERROR_SUCCESS; i++) {
            if (std::string(name).find(software) != std::string::npos) {
                RegCloseKey(hKey);
                return true;
            }
            nameSize = sizeof(name);
        }
        RegCloseKey(hKey);
    }
    return false;
}
#endif

bool DependencyManager::is_package_installed(const std::string& package) {
#ifdef _WIN32
    return check_registry_software(package) || is_command_available(package);
#else
    if (is_command_available(package)) return true;

    // Linux için farklı paket yöneticilerini kontrol et
    std::string checkCommands[] = {
        "dpkg -l " + package + " > /dev/null 2>&1",
        "rpm -q " + package + " > /dev/null 2>&1",
        "pacman -Q " + package + " > /dev/null 2>&1",
        "flatpak list | grep " + package + " > /dev/null 2>&1",
        "snap list | grep " + package + " > /dev/null 2>&1"
    };

    for (const auto& cmd : checkCommands) {
        if (std::system(cmd.c_str()) == 0) {
            return true;
        }
    }

    return false;
#endif
}