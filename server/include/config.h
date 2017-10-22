#pragma once

#include <string>

#include <boost/filesystem.hpp>
#include <spdlog/spdlog.h>

#include "utils.h"

using namespace boost::filesystem;

static const std::string CONFIG_FOLDER = Expand("~/.config/yamp/");

class Config {
public:
	Config();

	unsigned getPortNumber() const;
	std::string getBindAddress() const;
	
	std::string getPidFile()	const;
	std::string GetDbFile()		const;
	std::string GetLogFile()	const;
	path GetDir() const;
	bool GetAutostart() const;
	spdlog::level::level_enum GetLogLevel() const;

private:
	void Load();
	void LoadFile(const std::string& f);
	void Store(const std::string& f);

	struct Options {
			//TODO: ipv6
		unsigned portnumber		= 6600;
		std::string bindaddress	= "0.0.0.0";

		//File to store the pid number so we can check if the daemon is really running
		std::string pidfile 	= "/tmp/yamp.pid";

		//Playlist folder
		std::string playlistfolder = CONFIG_FOLDER+"playlist/";

		//Log file
		std::string logfile		= CONFIG_FOLDER+"log.txt";

		//Location of the songs
		path dir = ".";

		short loglevel = 0;
	}opt;
};
