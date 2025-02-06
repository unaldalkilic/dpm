#ifndef PACKER_HPP
#define PACKER_HPP

#include <string>

class Packer{
public:
    void pack(std::string& path);
    void depack(std::string& package_path);
    std::string get_dpmeta_string(std::string& package_path);
};

#endif