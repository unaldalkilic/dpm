#ifndef PARSER_HPP
#define PARSER_HPP

#include "parser_meta.hpp"

class Parser {
    public:
        Command parse(const int argc, const char* argv[]);
};

#endif