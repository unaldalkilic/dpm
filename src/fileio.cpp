#include "../include/fileio.hpp"
#include <filesystem>

std::string read_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("Unable to open file: " + filename);
    }
    std::ostringstream buffer;
    buffer << file.rdbuf();
    file.close();
    return buffer.str();
}

void write_file(const std::string& file_path, const std::string& content, bool append) {
    std::filesystem::path path(file_path);
    std::filesystem::path dir = path.parent_path();

    if (!dir.empty() && !std::filesystem::exists(dir)) {
        std::filesystem::create_directories(dir);
    }

    std::ofstream file(file_path, append ? std::ios::app : std::ios::trunc);

    if (!file) {
        throw std::runtime_error("File could not be opened or created!");
    }

    file << content << std::endl;

    file.close();
}