#ifndef PARSER_META_H
#define PARSER_META_H

#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <string>

enum class CommandType {
    INSTALL,
    REMOVE,
    UNKNOWN
};

enum class Option {
    YES
};

struct Command {
    CommandType command_type;
    std::unordered_set<Option> options;
    std::vector<std::string> arguments;
};

const std::unordered_map<std::string, CommandType> string_to_command_type = {
    {"install", CommandType::INSTALL},
    {"remove", CommandType::REMOVE}
};

const std::unordered_map<std::string, Option> string_to_option = {
    {"y", Option::YES}
};


#endif