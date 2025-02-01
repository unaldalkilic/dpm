#ifndef ENGINE_H
#define ENGINE_H

#include "parser_meta.h"

class Engine {
public:
    bool run(Command command);
};

const std::string help_string = "sample usage for dpm: dpm install <package_name>.dp -y";

#endif