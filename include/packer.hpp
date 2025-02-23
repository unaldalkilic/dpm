#ifndef PACKER_HPP
#define PACKER_HPP

#include <string>
#include "dictionary.hpp"
#include <unordered_set>
#include <cstdint>

class Packer{
public:
    void pack(const std::string& path);
    void depack(const std::string& package_path);
    Dictionary<std::string, std::string> get_dpmeta(const std::string& path);
    std::string get_package_dpmeta(const std::string& package_path);
private:
    bool validate_dpmeta(const std::string& path);
    bool validate_dpmeta(Dictionary<std::string, std::string> dpmeta_content_dictionary);
    Dictionary<std::string, std::string> read_dpmeta(const std::string& path);
    void write_platform_section(std::ofstream &out, const std::string& platform_path, uint64_t platform_section_offset = 0);
    Dictionary<std::string, std::string> dpmeta_string_to_dict(const std::string& dpmeta_string);
    bool is_dp_package(const std::string& package_path);

    struct FileEntry {
        std::string relative_path;
        std::string content;
        uint64_t relative_path_sizeof;
        uint64_t content_sizeof;
    };
};

const std::unordered_set<std::string> mandatory_dpmeta_fields = {
    "name", "version", "author", "author-mail"
};

const std::unordered_set<std::string> optional_dpmeta_fields = {
    "dependent", "dependent-linux", "dependent-win", "dependent-mac"
};

#endif