#ifndef PARSER_H
#define PARSER_H

#include "parser_meta.h"

class Parser {
    public:
        Command parse(const int argc, const char* argv[]);
};

#endif