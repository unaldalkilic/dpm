#include "../include/engine.hpp"
#include "../include/helpers.hpp"
#include <iostream>

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
    case CommandType::PACK:
        for (std::string path: command.arguments)
            try { packer.pack(get_cwd() + "/" + path); }
            catch (const std::exception &e) { print(e.what(), OutputType::ERROR); }
        break;
    case CommandType::DEPACK:
        for (std::string path: command.arguments) {
            const bool all = command.options.find(Option::ALL) != command.options.end();
            try { packer.depack(get_cwd() + "/" + path, all); }
            catch (const std::exception &e) { print(e.what(), OutputType::ERROR); }
        }
        break;
    case CommandType::DPMETA:
        if (command.options.find(Option::EXPORT) != command.options.end()) { // If export option found
            for (std::string package_path: command.arguments)
                try { packer.export_package_dpmeta(package_path); }
                catch (const std::exception &e) { print(e.what(), OutputType::ERROR); }   
            break;
        }

        for (std::string package_path: command.arguments)
            try { std::cout << packer.get_package_dpmeta(package_path) + "\n"; } 
            catch (const std::exception &e) { print(e.what(), OutputType::ERROR); }
        break;
    default:
        break;
    }

    return true;
}