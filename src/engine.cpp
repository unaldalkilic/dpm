#include <iostream>
#include "../include/engine.h"

bool Engine::run(Command command) {
    // First, look at which type the command is
    switch (command.command_type)
    {
    case CommandType::HELP:
        std::cout << help_string << "\n";
        break;
    
    default:
        break;
    }

    return true;
}