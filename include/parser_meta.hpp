#ifndef PARSER_META_HPP
#define PARSER_META_HPP

#include <unordered_set>
#include "dictionary.hpp"
#include <vector>
#include <string>

enum class CommandType {
    INSTALL,
    REMOVE,
    HELP,
    VERSION,
    PACK,
    DPMETA,
    NONE
};

enum class Option {
    YES,
    NONE
};

struct Command {
    CommandType command_type;
    std::unordered_set<Option> options;
    std::vector<std::string> arguments;
};

const Dictionary<std::string, CommandType> string_to_command_type({
    {"install", CommandType::INSTALL},
    {"remove", CommandType::REMOVE},
    {"help", CommandType::HELP},
    {"version", CommandType::VERSION},
    {"pack", CommandType::PACK},
    {"dpmeta", CommandType::DPMETA}
});


const Dictionary<std::string, Option> string_to_option({
    {"y", Option::YES},
    {"yes", Option::YES}
});


#endif