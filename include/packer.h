#ifndef PACKER_H
#define PACKER_H

#include <string>

class Packer{
public:
    bool pack(std::string& path);
    bool depack(std::string& package_path);
    std::string get_dpmeta_string(std::string& package_path);
};

#endif