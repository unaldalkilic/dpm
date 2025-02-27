#include <iostream>
#include "../include/helpers.hpp"
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/utsname.h>
#endif

std::string get_os_name() {
    #ifdef _WIN32
        return "win";
    #else
        struct utsname sysinfo;
        if (uname(&sysinfo) != 0) {
            return "unknown";
        }
        std::string os_name(sysinfo.sysname);
        if (os_name == "Darwin") {
            return "mac";
        } else if (os_name == "Linux") {
            return "linux";
        } else {
            return "unknown";
        }
    #endif
}

std::string get_cwd() {
    char cwd[1024];
    #ifdef _WIN32
        _getcwd(cwd, sizeof(cwd));
    #else
        getcwd(cwd, sizeof(cwd));
    #endif
    
    return std::string(cwd);
}

std::string get_filename(const std::string& path) {
    if (path.empty()) return "";

    size_t end = path.find_last_not_of("/\\");
    if (end == std::string::npos) return "";

    size_t last_slash = path.find_last_of("/\\", end); 

    return path.substr(last_slash == std::string::npos ? 0 : last_slash + 1, end - last_slash);
}

std::string get_raw_filename(const std::string& path) {
    std::string filename = get_filename(path);
    size_t last_dot = filename.find_last_of('.');

    if (last_dot != std::string::npos && last_dot > 0) 
        return filename.substr(0, last_dot);

    return filename;
}

std::string version_string_from_int_array(std::tuple<int*, int> version_tuple) {
    int* version_array = std::get<0>(version_tuple);
    int version_array_size = std::get<1>(version_tuple);
    
    std::string result = "";
    for (int i = 0; i < version_array_size; i++) {
        result += (std::to_string(version_array[i]) + ".");
    }
    result.pop_back();

    return result;
}

std::string stylish_string(const std::string& content, const OutputType type) {
    Colors output_color = output_type_colors_map.get(type).value_or(Colors::WHITE);
    return "\e[0;" + std::to_string(enum_to_integer(output_color)) + "m" + content + "\e[0m";
}

void print(const std::string& content, const OutputType type) {
    std::cout << stylish_string(content, type) << "\n";
}

std::vector<std::string> split(const std::string& content, const std::string& delimiter) {
    std::vector<std::string> result;

    if (delimiter.length() == 1) { 
        std::istringstream stream(content);
        std::string token;
        char delim = delimiter[0];
        
        while (std::getline(stream, token, delim)) {
            result.push_back(token);
        }
    } else {
        size_t start = 0, end;
        while ((end = content.find(delimiter, start)) != std::string::npos) {
            result.push_back(content.substr(start, end - start));
            start = end + delimiter.length();
        }
        result.push_back(content.substr(start));
    }

    return result;
}