#include <stdexcept>

#include <boost/filesystem.hpp>

#include <sys/stat.h>
#include <unistd.h>

#include "config.h"
#include "utils.h"

// Copied from the net.
void daemonize() {
    pid_t pid, sid;

    /* already a daemon */
    if(getppid() == 1) return;

    /* Fork off the parent process */
    pid = fork();
    if(pid < 0) {
        exit(EXIT_FAILURE);
    }
    /* If we got a good PID, then we can exit the parent process. */
    if(pid > 0) {
        exit(EXIT_SUCCESS);
    }

    /* At this point we are executing as the child process */

    /* Change the file mode mask */
    umask(0);

    /* Create a new SID for the child process */
    sid = setsid();
    if(sid < 0) {
        exit(EXIT_FAILURE);
    }

    /* Change the current working directory.  This prevents the current
       directory from being locked; hence not being able to remove it. */
    /*if ((chdir("/")) < 0) {
        exit(EXIT_FAILURE);
    }*/

    /* Redirect standard files to /dev/null */
    FILE* f = freopen("/dev/null", "r", stdin);
    if(!f) exit(EXIT_FAILURE);
    f = freopen("/dev/null", "w", stdout);
    if(!f) exit(EXIT_FAILURE);
    f = freopen("/dev/null", "w", stderr);
    if(!f) exit(EXIT_FAILURE);
}

std::ifstream::pos_type filesize(std::string filename) {
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg();
}

std::string Expand(std::string file) {
    auto pos = file.find('~');
    if(pos != std::string::npos) {
        file.erase(pos, 1);
        file.insert(pos, GetHome());
    }

    return file;
}

std::string GetHome() try {
    static std::string home = getenv("HOME");
    if(home.empty()) {
        throw std::runtime_error("HOME env variable not found, exiting");
    }
    return home;
} catch(...) {
    throw std::runtime_error("HOME env variable not found, exiting");
}

void ExecuteScript(const std::string& script_name) {
    auto script = CONFIG_FOLDER + script_name;
    if(exists(script)) {
        if(system(script.c_str()) == -1)
            spdlog::get("global")->info("Error executing {}", script);
        else
            spdlog::get("global")->trace("Script {} executed", script);
    } else {
        spdlog::get("global")->trace("Script {} doesn't exists", script);
    }
}