#include "song.hpp"
#include "config.hpp"

#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/taglib.h>

#include <spdlog/spdlog.h>

#include <boost/filesystem.hpp>

#include <fstream>
#include <mutex>

using namespace boost::filesystem;

static std::mutex song_mutex;

Song::Song(const path& p) : extension(p.extension().c_str()), file(p.c_str()) {
}

std::string Song::GetTitle() {
    std::lock_guard<std::mutex> song_guard(song_mutex);

    if(title != "") return title;

    std::string dbdir(CONFIG_FOLDER + "/.db/" + file);

    if(!exists(dbdir) && !create_directories(dbdir)) {
        spdlog::get("global")->warn("Could not create {} directory ", dbdir);
        GetTitleFromFile();
    } else {
        auto title_path = dbdir + "/TITLE";
        std::ifstream title_file(title_path);
        if(title_file.is_open()) {
            std::getline(title_file, title);
        } else {
            GetTitleFromFile();
            std::ofstream title_file(title_path);
            title_file << title << "\n";
        }
    }

    return title;
}

std::string Song::GetArtist() {
    std::lock_guard<std::mutex> song_guard(song_mutex);

    if(artist != "") return artist;

    std::string dbdir(CONFIG_FOLDER + "/.db/" + file);

    if(!exists(dbdir) && !create_directories(dbdir)) {
        spdlog::get("global")->warn("Could not create {} directory ", dbdir);
        GetArtistFromFile();
    } else {
        auto artist_path = dbdir + "/ARTIST";
        std::ifstream artist_file(artist_path);
        if(artist_file.is_open()) {
            std::getline(artist_file, artist);
        } else {
            GetArtistFromFile();
            std::ofstream artist_file(artist_path);
            artist_file << artist << "\n";
        }
    }

    return artist;
}

std::string Song::GetAlbum() {
    std::lock_guard<std::mutex> song_guard(song_mutex);
    if(album == "") {
        TagLib::FileRef f(file.c_str());
        if(f.isNull()) {
            artist = title = album = "Unknown";
        } else {
            auto tmp = f.tag()->album();
            if(tmp == TagLib::String::null) {
                tmp = TagLib::String("Unknown");
            }
            album = tmp.to8Bit(true);
        }
    }
    return artist;
}

std::string Song::GetFile() {
    std::lock_guard<std::mutex> song_guard(song_mutex);
    return file;
}

std::string Song::GetExtension() {
    std::lock_guard<std::mutex> song_guard(song_mutex);
    return extension;
}

unsigned Song::GetReproductions() {
    std::string dbdir(CONFIG_FOLDER + "/.db/" + file);

    if(!exists(dbdir) && !create_directories(dbdir)) {
        spdlog::get("global")->warn("Could not create {} directory ", dbdir);
        reproductions = 0;
    } else {
        auto counter_path = dbdir + "/COUNTER";
        std::ifstream counter_file(counter_path);
        if(counter_file.is_open()) {
            counter_file >> reproductions;
        } else {
            reproductions = 0;
            std::ofstream counter_file(counter_path);
            counter_file << reproductions << "\n";
        }
    }

    return reproductions;
}

void Song::SetReproductions(unsigned rep) {
    std::string dbdir(CONFIG_FOLDER + "/.db/" + file);

    if(!exists(dbdir) && !create_directories(dbdir)) {
        spdlog::get("global")->warn("Could not create {} directory ", dbdir);
    } else {
        auto counter_path = dbdir + "/COUNTER";
        std::ofstream counter_file(counter_path);
        counter_file << rep << "\n";
    }
}

void Song::GetTitleFromFile() {
    TagLib::FileRef f(file.c_str());
    if(f.isNull()) {
        artist = title = "Unknown";
    } else {
        auto tmp = f.tag()->title();
        if(tmp == TagLib::String::null) {
            tmp = TagLib::String("Unknown");
        }
        title = tmp.to8Bit(true);
    }
}

void Song::GetArtistFromFile() {
    TagLib::FileRef f(file.c_str());
    if(f.isNull()) {
        artist = title = "Unknown";
    } else {
        auto tmp = f.tag()->artist();
        if(tmp == TagLib::String::null) {
            tmp = TagLib::String("Unknown");
        }
        artist = tmp.to8Bit(true);
    }
}
