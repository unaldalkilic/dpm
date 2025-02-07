#include "../include/exceptions.hpp"
#include "../include/parser.hpp"

Command Parser::parse(const int argc, const char* argv[]){
    Command result;

    // Pre-condition for dpm to check whether the arguments given or not
    if (argc < 2)
        throw InvalidParameter("Error! You need to enter the command type such as install, remove etc.\n");

    // Always the first phrase after dpm is the command type, so check it
    std::string command_type_string = argv[1];
    CommandType command_type = string_to_command_type.get(command_type_string).value_or(CommandType::NONE);
    if (command_type == CommandType::NONE)
        throw InvalidParameter("Error! You need to enter a valid command type such as install, remove etc.\n");
    result.command_type = command_type;

    // Check every argument after the command and determine which are argument and option; and also ignore the others
    for (int i = 2; i < argc; i++) {
        std::string arg = argv[i];
        // Check options
        if (arg.rfind("-") == 0 || arg.rfind("--") == 0) {
            const int temp_substr_val = arg.rfind("-") == 0 ? 1 : 2;
            Option option = string_to_option.get(arg.substr(temp_substr_val)).value_or(Option::NONE);
            if (option == Option::NONE)
                continue;
        }
        else 
            result.arguments.push_back(arg);
        
    }

    return result;
}