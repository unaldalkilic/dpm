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
    bool has_windows = std::filesystem::exists(path + "/win") && std::filesystem::is_directory(path + "/win");
    bool has_mac = std::filesystem::exists(path + "/mac") && std::filesystem::is_directory(path + "/mac");
    bool has_common = std::filesystem::exists(path + "/common") && std::filesystem::is_directory(path + "/common");
    if (! has_linux && ! has_windows && ! has_mac)
        throw InvalidPackage("Given path " + path + " does not have linux, win or mac folder; not suitable to become .dp package. It needs to support one of the platforms at least.");

    if(has_linux && (! std::filesystem::exists(path + "/linux/src") || ! std::filesystem::is_directory(path + "/linux/src")))
        throw InvalidPackage("Given path " + path + " does not have src folder under linux folder; not suitable to become .dp package. It needs to have src folder that contains the package source folder for linux platform");
    if(has_windows && (! std::filesystem::exists(path + "/win/src") || ! std::filesystem::is_directory(path + "/win/src")))
        throw InvalidPackage("Given path " + path + " does not have src folder under win folder; not suitable to become .dp package. It needs to have src folder that contains the package source folder for windows platform");   
    if(has_mac && (! std::filesystem::exists(path + "/mac/src") || ! std::filesystem::is_directory(path + "/mac/src")))
        throw InvalidPackage("Given path " + path + " does not have src folder under mac folder; not suitable to become .dp package. It needs to have src folder that contains the package source folder for mac platform");
        
    // Validate dpmeta file //
    if (! validate_dpmeta(path + "/dpmeta"))
        throw InvalidPackage("Given path's (" + path + ") dpmeta is invalid!");

    // Binary write .dp package section //
    std::string dp_path = get_cwd() + "/out.dp";
    std::ofstream dp_file (dp_path, std::ios::binary);
    if (!dp_file)
        throw std::runtime_error(dp_path+ " Package cannot be created!");

    dp_file.write("DPMXPACK", 8); // magic 8 bytes
    
    // dpmeta sizeof
    std::string dpmeta_content = read_file(path + "/dpmeta");
    uint64_t dpmeta_sizeof = dpmeta_content.size();
    dp_file.write(reinterpret_cast<const char*>(&dpmeta_sizeof), sizeof(uint64_t));

    // Platform and common folder offsets, initialize 8 byte 0's, then fill properly
    uint64_t linux_offset = 0, win_offset = 0, mac_offset = 0, common_offset = 0;
    dp_file.write(reinterpret_cast<const char*>(&linux_offset), sizeof(uint64_t));
    dp_file.write(reinterpret_cast<const char*>(&win_offset), sizeof(uint64_t));
    dp_file.write(reinterpret_cast<const char*>(&mac_offset), sizeof(uint64_t));
    dp_file.write(reinterpret_cast<const char*>(&common_offset), sizeof(uint64_t));

    dp_file.write(dpmeta_content.data(), dpmeta_sizeof); // dpmeta file content

    uint64_t current_offset = dp_file.tellp(); // gets the current offset position

    // Write the contents of each platform //
    if (has_linux) {
        linux_offset = current_offset;
        write_platform_section(dp_file, path + "/linux");
        current_offset = dp_file.tellp();
    }
    if (has_windows) {
        win_offset = current_offset;
        write_platform_section(dp_file, path + "/win");
        current_offset = dp_file.tellp();
    }
    if (has_mac) {
        mac_offset = current_offset;
        write_platform_section(dp_file, path + "/mac");
        current_offset = dp_file.tellp();
    }
    if (has_common) {
        common_offset = current_offset;
        write_platform_section(dp_file, path + "/common");
        current_offset = dp_file.tellp();
    }

    // Update the linux, windows, mac and common offsets located on the beginning of the .dp package //
    dp_file.seekp(16); // 16th position because magic(8) + dpmeta_sizeof(8), then offsets begin in given order above comment
    dp_file.write(reinterpret_cast<const char*>(&linux_offset), sizeof(uint64_t));
    dp_file.write(reinterpret_cast<const char*>(&win_offset), sizeof(uint64_t));
    dp_file.write(reinterpret_cast<const char*>(&mac_offset), sizeof(uint64_t));
    dp_file.write(reinterpret_cast<const char*>(&common_offset), sizeof(uint64_t));

    dp_file.close();
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
    return validate_dpmeta(content);
}

bool Packer::validate_dpmeta(Dictionary<std::string, std::string> dpmeta_content_dictionary) {
    for (std::string mandatory_field: mandatory_dpmeta_fields) {
        if (dpmeta_content_dictionary.get(mandatory_field) == std::nullopt)
            return false;
    }
    return true;
}

void Packer::write_platform_section(std::ofstream &out, const std::string& platform_path, uint64_t platform_section_offset) {
    const std::vector<std::string> script_names = {"pre_install", "post_install", "pre_remove", "post_remove"};
    
    // Set the out or the platform section offset properly, it is an optional argument.
    if (platform_section_offset == 0)
        platform_section_offset = out.tellp();
    else
        out.seekp(platform_section_offset);
    
    uint64_t source_offset = 0; // Initialize source offset as 0, then we will update this and rewrite
    out.write(reinterpret_cast<const char*>(&source_offset), sizeof(uint64_t));

    // Write scripts sizes and their contents in given order above (script_names variable)
    for (std::string script_name: script_names) {
        std::string script_path = platform_path + "/" + script_name;
        uint64_t script_sizeof = 0; // Initialize as 0, then update

        if (! std::filesystem::exists(script_path)) {
            out.write(reinterpret_cast<const char*>(&script_sizeof), sizeof(uint64_t));
            continue;
        } 

        std::string script_content = read_file(script_path);
        script_sizeof = script_content.size();
        out.write(reinterpret_cast<const char*>(&script_sizeof), sizeof(uint64_t));
        out.write(script_content.data(), script_sizeof);
    }

    source_offset = out.tellp();
    out.seekp(platform_section_offset);
    out.write(reinterpret_cast<const char*>(&source_offset), sizeof(uint64_t)); // Update the offset value of source folder of platform
    out.seekp(source_offset); // Return back after updating the offset value, ready to fill source
    
    // Fill the source section of the platform //
    const std::string source_path = platform_path + "/src";
    if (! std::filesystem::exists(source_path) || ! std::filesystem::is_directory(source_path))
        return;

    for (const auto &entry: std::filesystem::recursive_directory_iterator(platform_path)) {
        if (entry.is_regular_file()) {
            FileEntry file_entry;
            file_entry.relative_path = std::filesystem::relative(entry.path(), platform_path).string(); // Get relative path in terms of platform
            file_entry.content = read_file(entry.path().string());
            file_entry.content_sizeof = file_entry.content.size();
            file_entry.relative_path_sizeof = file_entry.relative_path.size();
            
            out.write(file_entry.relative_path.data(), file_entry.relative_path_sizeof);
            out.write(file_entry.content.data(), file_entry.content_sizeof);
        }
    }
}