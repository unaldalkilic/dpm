#ifndef PACKER_HPP
#define PACKER_HPP

#include <string>
#include "dictionary.hpp"
#include <unordered_set>

class Packer{
public:
    void pack(const std::string& path);
    void depack(const std::string& package_path);
    Dictionary<std::string, std::string> get_dpmeta(const std::string& package_path);
private:
    bool validate_dpmeta(const std::string& path);
    bool validate_dpmeta(Dictionary<std::string, std::string> dpmeta_content_dictionary);
    Dictionary<std::string, std::string> read_dpmeta(const std::string& path);
};

const std::unordered_set<std::string> mandatory_dpmeta_fields = {
    "name", "version", "author", "author-mail"
};

const std::unordered_set<std::string> optional_dpmeta_fields = {
    "dependent", "dependent-linux", "dependent-windows"
};

#endif