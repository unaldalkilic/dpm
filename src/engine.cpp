#include "../include/engine.hpp"
#include "../include/helpers.hpp"

bool Engine::run(Command command) {
    // First, look at which type the command is
    switch (command.command_type)
    {
    case CommandType::HELP:
        print(help_string, OutputType::INFO);
        break;
    case CommandType::VERSION:
        print(version_string_from_int_array(dpm_version), OutputType::INFO); // Later, user can query the version of .dp packages
        break;
    default:
        break;
    }

    return true;
}