#include <iostream>
#include "../include/parser.h"

Command Parser::parse(const int argc, const char* argv[]){
    Command result;

    // Pre-condition for dpm to check whether the arguments given or not
    if (argc < 2) {
        std::cerr << "Error! You need to enter the command type such as install, remove etc.\n";
        return result;
    }

    // Always the first phrase after dpm is the command type, so check it
    std::string command_type_string = argv[1];
    auto command_type = string_to_command_type.find(command_type_string);
    if (command_type != string_to_command_type.end())
        result.command_type = command_type->second;
    else {
        std::cerr << "Error! You need to enter a valid command type such as install, remove etc.\n";
        result.command_type = CommandType::UNKNOWN;
        return result;
    }

    // Check every argument after the command and determine which are argument and option; and also ignore the others
    for (int i = 2; i < argc; i++) {
        std::string arg = argv[i];
        // Check options
        if (arg.rfind("-") == 0) {
            auto option = string_to_option.find(arg.substr(1));
            if (option != string_to_option.end()) 
                result.options.insert(option->second);
            else 
                continue;
        }
        // else if (arg.rfind("--") == 0) {}
        // Check arguments
        else 
            result.arguments.push_back(arg);
        
    }

    return result;
}