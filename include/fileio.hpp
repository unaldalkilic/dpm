#ifndef FILEIO_HPP
#define FILEIO_HPP

#include <string>
#include <fstream>
#include <sstream>

std::string read_file(const std::string& filename);
void write_file(const std::string& file_path, const std::string& content, bool append = false);

#endif