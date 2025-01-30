#ifndef PARSER_TESTS_H
#define PARSER_TESTS_H

#include <iostream>
#include "../include/parser/parser.h"

const std::unordered_map<CommandType, std::string> command_type_to_string = {
    {CommandType::INSTALL, "install"},
    {CommandType::REMOVE, "remove"}
};

const std::unordered_map<Option, std::string> option_to_string = {
    {Option::YES, "yes"}
};

void basic_parser_test(const int argc, const char* argv[]);

#endif