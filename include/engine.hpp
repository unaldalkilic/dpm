#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "parser_meta.hpp"
#include <tuple>

class Engine {
public:
    bool run(Command command);
};

const std::tuple<int*, int> dpm_version(new int[3]{1,0,0}, 3);
const std::string help_string = "sample usage for dpm: dpm install <package_name>.dp -y";

#endif