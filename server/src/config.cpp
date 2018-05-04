#include <fstream>
#include <iostream>
#include <mutex>

#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

#include "config.hpp"

static std::mutex config_mutex;

Config::Config() {
    path dir(CONFIG_FOLDER);
    if(!exists(dir) && !create_directory(dir))
        throw std::runtime_error("Could not create config directory");

    Load();
}

unsigned Config::getPortNumber() const {
    std::lock_guard<std::mutex> lock(config_mutex);
    return opt.portnumber;
}

std::string Config::getBindAddress() const {
    std::lock_guard<std::mutex> lock(config_mutex);
    return opt.bindaddress;
}

std::string Config::getPidFile() const {
    std::lock_guard<std::mutex> lock(config_mutex);
    return opt.pidfile;
}

std::string Config::GetLogFile() const {
    std::lock_guard<std::mutex> lock(config_mutex);
    return opt.logfile;
}

spdlog::level::level_enum Config::GetLogLevel() const {
    std::lock_guard<std::mutex> lock(config_mutex);
    return static_cast<spdlog::level::level_enum>(opt.loglevel);
}

// Private functions

void Config::LoadFile(const std::string& f) {
    spdlog::get("global")->info("Reading config file {}", f);

    YAML::Node config = YAML::LoadFile(f);

    if(auto e = config["pid_file"]) opt.pidfile = e.as<std::string>();
    if(auto e = config["log_file"]) opt.logfile = e.as<std::string>();
    if(auto e = config["log_level"]) opt.loglevel = e.as<short>();

    if(auto e = config["port_number"]) opt.portnumber = e.as<unsigned>();
    if(auto e = config["bind_address"]) opt.bindaddress = e.as<std::string>();
}

void Config::Store(const std::string& f) {
    YAML::Node config;
    config["pid_file"] = opt.pidfile;
    config["log_file"] = opt.logfile;
    config["log_level"] = opt.loglevel;

    config["port_number"] = opt.portnumber;
    config["bind_address"] = opt.bindaddress;

    std::ofstream config_file(f);
    config_file << config;
}

void Config::Load() {
    const auto config_file = CONFIG_FOLDER + "server.yml";
    try {
        LoadFile(config_file);
    } catch(YAML::BadFile& e) {
        spdlog::get("global")->warn("Config file not found, creating {}", config_file);
        Store(config_file);
    }

    opt.pidfile = Expand(opt.pidfile);
    opt.logfile = Expand(opt.logfile);
}
