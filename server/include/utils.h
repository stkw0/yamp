#pragma once

/// @file utils.h
/// @brief Some misc useful utils that don't deserve an own class

#include <fstream>
#include <string>

/// @brief Daemonize the server
/// @note We can not change the current directory because it is possible 
/// that we need to find the songs of the current directory.
/// @see Original: http://www.itp.uzh.ch/~dpotter/howto/daemonize
void daemonize();

/// @brief Get the size of a given file
/// @param filename Path to the file
/// @return Size of the file in bytes
/// @see Original: http://stackoverflow.com/a/5840160
std::ifstream::pos_type filesize(std::string filename);

std::string Expand(std::string file);
std::string GetHome();
void ExecuteScript(const std::string& script_name);
