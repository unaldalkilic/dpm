#ifndef HELPERS_HPP
#define HELPERS_HPP

#include <string>
#include <tuple>
#include <vector>
#include "dictionary.hpp"
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
    WHITE = 37
};

std::string get_os_name();
std::string get_cwd();
std::string get_raw_filename(const std::string& path);
std::string get_filename(const std::string& path);
std::string version_string_from_int_array(std::tuple<int*, int> version_tuple);
std::string string(const std::string& content, const OutputType type = OutputType::NONE);
void print(const std::string& content, const OutputType type = OutputType::NONE);
std::vector<std::string> split(const std::string& content, const std::string& delimeter = " ");

const Dictionary<OutputType, Colors> output_type_colors_map({
    {OutputType::NONE, Colors::WHITE},
    {OutputType::ERROR, Colors::RED},
    {OutputType::INFO, Colors::BLUE},
    {OutputType::SUCCESS, Colors::GREEN},
    {OutputType::WARNING, Colors::YELLOW}
});

template <typename Enumeration>
auto enum_to_integer(Enumeration const value)
    -> typename std::underlying_type<Enumeration>::type
{
    return static_cast<typename std::underlying_type<Enumeration>::type>(value);
}

#endif