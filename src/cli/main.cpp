#include <iostream>
#include <filesystem>
#include <fstream>
#include "sender.h"
#include <nlohmann/json.hpp>
#include <argparse/argparse.hpp>

using str = std::string;
using json = nlohmann::json;
namespace fs = std::filesystem;

void splitByHyphen(const str& input, str& left, str& right) {
    size_t hyphenPos = input.find('-');
    if (hyphenPos != str::npos) {
        left = input.substr(0, hyphenPos);
        right = input.substr(hyphenPos + 1);
    } else {
        std::cerr << "No hyphen found in the input string." << std::endl;
    }
}


json cli(const str& command, const str& subcommand, const str& path, const str& name, const str& version, const str& new_name, const str& new_version, const str& new_wd_name) {
    json jsonData;

    try {
        if (command == "ep") {
            if (subcommand == "list") {
                jsonData["type"] = "get_env_packs";            
            } else if (subcommand == "create") {
                jsonData["type"] = "create_env_pack";
                jsonData["args"]["new_ep_dir"] = path;            
            } else if (subcommand == "delete") {
                jsonData["type"] = "delete_env_pack";
                jsonData["args"]["ep_name"] = name;
                jsonData["args"]["ep_version"] = version;
            } else if (subcommand == "rename") {
                jsonData["type"] = "rename_env_pack";
                jsonData["args"]["ep_name"] = name;
                jsonData["args"]["ep_version"] = version;
                jsonData["args"]["new_ep_name"] = new_name;
                jsonData["args"]["new_ep_version"] = new_version;
            }
            
        }
        else if (command == "gi")
        {
            if (subcommand == "list"){
                jsonData["type"] = "get_game_instance";
            }
        }
        else if (command == "wd")
        {
            if (subcommand == "list"){
                jsonData["type"] = "get_world_data";
            }else if (subcommand == "import")
            {
                jsonData["type"] = "import_wd";
                jsonData["args"]["new_ed_dir"] = path;  
                jsonData["args"]["wd_name"] = new_wd_name;
            }
        }
        else {
            std::cout << "Unexpected input! Available: ep. You: " << command << std::endl;
            jsonData["error"] = "Invalid command";
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    }
    std::cout << "Now the JSON is..." << jsonData.dump(4) << std::endl;
    return jsonData;
}

int main(int argc, const char** argv) {
    NerMCManager::Sender sender;

    argparse::ArgumentParser program("nmm");

    program.add_argument("command")
        .help("command to execute (ep)");

    program.add_argument("subcommand")
        .help("subcommand to execute (list, create, delete, rename)");

    program.add_argument("path")
        .help("path for create ")
        .default_value("");

    program.add_argument("-t", "name-version", "new-name-version")
        .help("new name and version for rename")
        .default_value("");

    program.add_argument("-name", "wd-name")
        .help("wd name for import")
        .default_value("");

    try {
        program.parse_args(argc, argv);
    } catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return 1;
    }

    str command = program.get<str>("command");
    str subcommand = program.get<str>("subcommand");
    str path = program.get<str>("path");
    str name_version = program.get<str>("name-version");
    str new_name_version = program.get<str>("new-name-version");
    str new_wd_name = program.get<str>("wd-name");
    
    str name = "";
    str version = "";
    str new_name = "";
    str new_version = "";

    splitByHyphen(name_version, name, version);
    splitByHyphen(new_name_version, new_name, new_version);

    json request_content = cli(command, subcommand, path, name, version, new_name, new_version, new_wd_name);

    json response = sender.request(request_content);

    std::cout << "Response received: " << response.dump(4) << std::endl;

    return 0;
}