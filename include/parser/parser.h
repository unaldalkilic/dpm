#ifndef PARSER_H
#define PARSER_H

#include "parser_meta.h"

class Parser {
    public:
        Command parse(const int argc, const char* argv[]);

        const std::string help_string = 
            "sample usage for dpm: dpm install <package_name>.dp -y";
};

#endif