#include <iostream>
#include "../include/engine.hpp"
#include "../include/helpers.hpp"

bool Engine::run(Command command) {
    // First, look at which type the command is
    switch (command.command_type)
    {
    case CommandType::HELP:
        std::cout << help_string << "\n";
        break;
    case CommandType::VERSION:
        std::cout << version_string_from_int_array(dpm_version) << "\n"; // Later, user can query the version of .dp packages
        break;
    default:
        break;
    }

    return true;
}