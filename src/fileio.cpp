#include "../include/fileio.hpp"

std::string read_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("Unable to open file: " + filename);
    }
    std::ostringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void write_file(const std::string& file_path, const std::string& content, bool append) {
    std::ofstream file(file_path, append ? std::ios::app : std::ios::trunc);

    if (!file) {
        throw std::runtime_error("File could not opened or created!");
    }

    file << content << std::endl;
}