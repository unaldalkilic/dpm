#include "../include/packer.hpp"
#include "../include/exceptions.hpp"
#include <filesystem>
#include "../include/fileio.hpp"
#include "../include/helpers.hpp"

void Packer::pack(const std::string& path) {
    // Validate files and folders //
    if (! std::filesystem::exists(path) || ! std::filesystem::is_directory(path))
        throw TargetNotExist("Given path " + path + " does not exist or not a folder!");
    
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
    std::string dp_path = get_cwd() + "/" + get_filename(path) + ".dp";
    std::ofstream dp_file (dp_path, std::ios::binary);
    if (!dp_file)
        throw std::runtime_error(dp_path+ " package cannot be created!");

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

void Packer::depack(const std::string& package_path) {
    if (! is_dp_package(package_path)) throw InvalidParameter("Given " + package_path + " package is not a .dp package!");

    const std::string folder_name = get_raw_filename(package_path);
    export_package_dpmeta(package_path); // Export dpmeta first

    // Export the rest of the platform source folders and common folder if exist //
    std::ifstream dp_file(package_path, std::ios::binary);
    if (!dp_file) throw std::runtime_error("Cannot read the dp file!");

    dp_file.seekg(16); // Set the read position to the linux offset section
    Dictionary<std::string, uint64_t> platform_offset_dict;
    for (int i = 0; i < 4; i++) { // preferred hardcoded 4 value because cannot tolarate any dependency on a variable
        uint64_t platform_offset;
        dp_file.read(reinterpret_cast<char*>(&platform_offset), sizeof(uint64_t));
        if (dp_file.gcount() != static_cast<std::streamsize>(sizeof(uint64_t))) throw std::runtime_error("Failed to read platform metas of the package!");
        platform_offset_dict.add(dp_platforms[i], platform_offset);
    }

    for (std::string platform_name: platform_offset_dict.keys()) {
        uint64_t platform_offset = platform_offset_dict.get(platform_name).value_or(0);
        if (platform_offset == 0) continue; // platform does not exist for this package
        if (! std::filesystem::create_directories(folder_name+"/"+platform_name)) throw std::runtime_error("Cannot create the platform folder while exporting the package!");
        export_platform_section(dp_file, folder_name+"/"+platform_name, platform_offset);
    }

    dp_file.close();
}

Dictionary<std::string, std::string> Packer::read_dpmeta(const std::string& path) {
    if (! std::filesystem::exists(path))
        throw TargetNotExist("Given path " + path + " does not exist! So cannot read the dpmeta");

    std::string content = read_file(path);
    return dpmeta_string_to_dict(content);
}

Dictionary<std::string, std::string> Packer::dpmeta_string_to_dict(const std::string& dpmeta_string) {
    Dictionary<std::string, std::string> result;

    std::vector<std::string> lines = split(dpmeta_string, "\n");
    for (std::string line: lines) {
        std::vector<std::string> parts = split(line, ":");
        if (parts.size() != 2)
            throw InvalidPackage("Given dpmeta file is in wrong format!");
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
    // Set the out or the platform section offset properly, it is an optional argument.
    if (platform_section_offset == 0)
        platform_section_offset = out.tellp();
    else
        out.seekp(platform_section_offset);
    
    uint64_t source_offset = 0; // Initialize source offset as 0, then we will update this and rewrite
    out.write(reinterpret_cast<const char*>(&source_offset), sizeof(uint64_t));

    // Write scripts sizes and their contents in given order above (dp_script_names variable)
    for (std::string script_name: dp_script_names) {
        std::string script_path = platform_path + "/" + script_name + (get_raw_filename(platform_path) == "win" ? ".bat" : "");
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
    const std::string source_path = platform_path + (get_raw_filename(platform_path) != "common" ? "/src" : "");
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

void Packer::export_platform_section(std::ifstream &in, const std::string& platform_path, const uint64_t platform_offset) {
    in.seekg(platform_offset);
    uint64_t src_offset;
    in.read(reinterpret_cast<char*>(&src_offset), sizeof(uint64_t));

    for (std::string script_name: dp_script_names) {
        std::string script_content = get_script_content(in, script_name, platform_offset);
        if (script_content == "") continue;
        write_file(platform_path+"/"+script_name+(get_raw_filename(platform_path) == "win" ? ".bat" : ""), script_content);
    }

    in.seekg(src_offset);
    // src section TODO
}

std::string Packer::get_package_dpmeta(const std::string& package_path) {
    if (! is_dp_package(package_path))
        throw InvalidPackage("Package " + package_path + " is not a .dp package!");
    
    std::ifstream dp_file(package_path, std::ios::binary);
    if (! dp_file) throw std::runtime_error(package_path + " package cannot be read!");

    dp_file.seekg(8); // Change position to dpmeta sizeof section
    uint64_t dpmeta_sizeof;
    dp_file.read(reinterpret_cast<char*>(&dpmeta_sizeof), sizeof(uint64_t));
    if (dp_file.gcount() != static_cast<std::streamsize>(sizeof(uint64_t))) throw std::runtime_error("Failed to read dpmeta of the package!");

    dp_file.seekg(48); // change position to dpmeta content section
    char dpmeta_content[dpmeta_sizeof];
    dp_file.read(dpmeta_content, dpmeta_sizeof);
    if (dp_file.gcount() != static_cast<std::streamsize>(dpmeta_sizeof)) throw std::runtime_error("Failed to read dpmeta of the package!");

    return std::string(dpmeta_content, dpmeta_sizeof);
}

bool Packer::is_dp_package(const std::string& package_path) {
    if (! std::filesystem::exists(package_path)) 
        throw TargetNotExist("Given package path "+ package_path+" does not exists!");

    if (package_path.size() < 3 || package_path.rfind('.') == std::string::npos || package_path.substr(package_path.rfind('.')) != ".dp")  
        throw InvalidPackage("Given package " + package_path + " has invalid extension! It must be .dp");

    std::ifstream dp_file(package_path, std::ios::binary);
    if (! dp_file)
        throw std::runtime_error(package_path + " package cannot be read!");

    // Read and check the magic bytes, it must be DPMXPACK //
    char magic[8];
    dp_file.read(magic, 8);
    if (dp_file.gcount() != static_cast<std::streamsize>(8)) throw std::runtime_error("Failed to read given .dp package magic bytes!");

    dp_file.close();

    return std::string(magic, 8) == "DPMXPACK";
}

void Packer::export_package_dpmeta(const std::string& package_path) {
    const std::string dpmeta_content = get_package_dpmeta(package_path);
    const std::string folder_name = get_raw_filename(package_path);
    if (! std::filesystem::create_directories(folder_name)) throw std::runtime_error("Cannot exported the package folder!");
    write_file("./"+folder_name+"/dpmeta", dpmeta_content);
}

std::string Packer::get_script_content(std::ifstream &in, const std::string& script_name, const uint64_t platform_offset) {
    uint64_t script_offset = platform_offset + 8; // script offsets starts from here relatively to its platform offset
    in.seekg(script_offset);

    for (std::string dp_script_name: dp_script_names) {
        uint64_t script_sizeof;
        in.read(reinterpret_cast<char*>(&script_sizeof), sizeof(uint64_t));

        if (dp_script_name == script_name) {
            if (script_sizeof == 0) return "";

            char script_content[script_sizeof];
            in.read(script_content, script_sizeof);
            return std::string(script_content, script_sizeof);
        } else 
            in.seekg(in.tellg() + static_cast<std::streamoff>(script_sizeof));
    }
    throw InvalidParameter("Invalid script_name argument given!");
}