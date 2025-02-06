#include "../include/packer.hpp"
#include "../include/exceptions.hpp"
#include <filesystem>

void Packer::pack(std::string& path) {
    if (! std::filesystem::is_directory(path))
        throw TargetNotExist("Given path " + path + " does not exist!");
}