#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <thread>

#include <boost/filesystem.hpp>
#include <spdlog/spdlog.h>

#include "commands.h"
#include "manager.h"
#include "utils.h"

using namespace boost::filesystem;

// Public functions

Manager::Manager(int argc, char* argv[]) {
    if(argc == 2 && argv[1] == std::string("-d")) {
        daemonize();

        spdlog::drop("global");
        auto logging = spdlog::basic_logger_mt("global", conf.GetLogFile(), true);
        logging->info("Daemonazing server");

    } else {
        std::cout
            << "If you want to run it as a daemon restart it with \"-d\" flag"
            << std::endl;
    }
    spdlog::get("global")->set_level(conf.GetLogLevel());

    // Write the pid number
    std::ofstream opid_file(conf.getPidFile(), std::ios::trunc | std::ios::out);
    if(!opid_file.is_open()) {
        throw std::runtime_error("Pid file could not be opened");
    }

    opid_file << getpid();
    opid_file.close();
}

Manager::~Manager() {
    std::remove(conf.getPidFile().c_str());
}

void Manager::StartServer() {
    // Music music;

    // music.GetList().LoadDir(conf.GetDir());
    // std::thread mplayer([&music] { music.PlayList(); });

    // CommandControler cmd(music);

    // ExecuteScript("init.sh");

    RunServer();
    // mplayer.join();

    // ExecuteScript("exit.sh");
}

// Private Functions
/*
template <typename T>
void Manager::ProcessCommand(T& proto, CommandControler& cmd) {
    cmd.Execute(proto.ReadCommand(), proto);
}*/
