#include <iostream>
#include <string>

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include "config.hpp"
#include "manager.hpp"

int main(int argc, char* argv[]) try {
    auto logging = spdlog::stdout_color_mt("global");
    logging->set_level(spdlog::level::trace);

    // Check if we have some pid number
    std::ifstream ipid_file(Config().getPidFile());
    if(ipid_file.is_open()) {

        std::string pid;
        ipid_file >> pid;

        // Check if that pid is a real process
        std::ifstream f("/proc/" + pid + "/comm");
        if(f.is_open()) {
            std::string comm;
            f >> comm;
            if(comm == "yampd") {
                throw std::runtime_error("Server is already running");
            }
        }
    }

    logging->info("Starting server");

    Manager manager(argc, argv);
    manager.StartServer();
} catch(std::exception& e) {
    spdlog::get("global")->critical(e.what());
}
