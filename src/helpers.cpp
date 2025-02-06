#include <iostream>
#include "../include/helpers.hpp"

std::string get_cwd() {
    char cwd[1024];
    #ifdef _WIN32
        _getcwd(cwd, sizeof(cwd));
    #else
        getcwd(cwd, sizeof(cwd));
    #endif
    
    return std::string(cwd);
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

void print(std::string& content, OutputType type) {
    if (type == OutputType::NONE)
        std::cout << content << "\n";
    else
        std::cout << "\n";
        // std::cout << "\e[0;" << output_type_colors_map << "m"; TODO continue from here
}