#include "../include/packer.hpp"
#include "../include/exceptions.hpp"
#include <filesystem>
#include "../include/fileio.hpp"
#include "../include/helpers.hpp"

void Packer::pack(const std::string& path) {
    // Validate files and folders //
    if (! std::filesystem::exists(path) || ! std::filesystem::is_directory(path))
        throw TargetNotExist("Given path " + path + " does not exist!");
    
    if (! std::filesystem::exists(path + "/dpmeta"))
        throw InvalidPackage("Given path " + path + " does not have any dpmeta file; not suitable to become .dp package");

    bool has_linux = std::filesystem::exists(path + "/linux") && std::filesystem::is_directory(path + "/linux");
    bool has_windows = std::filesystem::exists(path + "/windows") && std::filesystem::is_directory(path + "/windows");
    if (! has_linux && ! has_windows)
        throw InvalidPackage("Given path " + path + " does not have linux or windows folder; not suitable to become .dp package. It needs to support one of the platforms at least.");

    if(has_linux && (! std::filesystem::exists(path + "/linux/src") || ! std::filesystem::is_directory(path + "/linux/src")))
        throw InvalidPackage("Given path " + path + " does not have src folder under linux folder; not suitable to become .dp package. It needs to have src folder that contains the package source folder for linux platform");
    if(has_windows && (! std::filesystem::exists(path + "/windows/src") || ! std::filesystem::is_directory(path + "/windows/src")))
        throw InvalidPackage("Given path " + path + " does not have src folder under windows folder; not suitable to become .dp package. It needs to have src folder that contains the package source folder for windows platform");   
        
    // Validate dpmeta file //
    if (! validate_dpmeta(path + "/dpmeta"))
        throw InvalidPackage("Given path's (" + path + ") dpmeta is invalid!");

    // TODO make it zip and then change the extension as .dp
}

Dictionary<std::string, std::string> Packer::read_dpmeta(const std::string& path) {
    if (! std::filesystem::exists(path))
        throw TargetNotExist("Given path " + path + " does not exist! So cannot read the dpmeta");

    Dictionary<std::string, std::string> result;
    std::string content = read_file(path);

    std::vector<std::string> lines = split(content, "\n");
    for (std::string line: lines) {
        std::vector<std::string> parts = split(line, ":");
        if (parts.size() != 2)
            throw InvalidPackage("Given dpmeta file " + path + " is in wrong format!");
        result.add(parts[0], parts[1]);
    }

    return result;
}

bool Packer::validate_dpmeta(const std::string& path) {
    Dictionary<std::string, std::string> content = read_dpmeta(path);
    for (std::string mandatory_field: mandatory_dpmeta_fields) {
        if (content.get(mandatory_field) == std::nullopt)
            return false;
    }
    return true;
}