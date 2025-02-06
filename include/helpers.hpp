#ifndef HELPERS_HPP
#define HELPERS_HPP

#include <string>
#include <tuple>
#include "data.hpp"
#ifdef _WIN32
#include <dirent.h>
#else
#include <unistd.h>
#endif

enum class OutputType {
    ERROR,
    SUCCESS,
    INFO,
    WARNING,
    NONE
};

enum class Colors { // Check for ansii color codes
    RED = 31,
    GREEN = 32,
    YELLOW = 33,
    BLUE = 34,
};

std::string get_cwd();
std::string version_string_from_int_array(std::tuple<int*, int> version_tuple);
void print(std::string& content, OutputType type = OutputType::NONE);


const Dictionary<OutputType, Colors> output_type_colors_map({
    {OutputType::ERROR, Colors::RED},
    {OutputType::INFO, Colors::BLUE},
    {OutputType::SUCCESS, Colors::GREEN},
    {OutputType::WARNING, Colors::YELLOW}
});

#endif