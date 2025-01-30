#include "parser_tests.h"

void basic_parser_test(const int argc, const char* argv[]) {
    Parser parser;
    Command command = parser.parse(argc, argv);

    if (command.command_type != CommandType::UNKNOWN)
        std::cout << "Command type is: " << command_type_to_string.find(command.command_type)->second << "\n";
    std::cout << "Options: ";
    for (Option option: command.options) 
        std::cout << option_to_string.find(option)->second << " ";
    std::cout << "\n";
    std::cout << "Arguments: ";
    for (std::string arg: command.arguments)
        std::cout << arg << " ";
    std::cout << "\n";
}