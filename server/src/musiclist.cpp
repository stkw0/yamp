#include "musiclist.h"

#include <spdlog/spdlog.h>

#include <array>
#include <boost/range/iterator_range.hpp>
#include <fstream>
#include <iostream>
#include <random>
#include <set>

// Public functions

void MusicList::LoadDir(path p) {
    if(!is_directory(p)) {
        throw std::runtime_error("Can not open the directory");
    }

    for(auto& entry : boost::make_iterator_range(directory_iterator(p), {})) {
        const auto pathSong = entry.path();
        if(is_directory(pathSong))
            LoadDir(pathSong);
        else
            LoadFile(pathSong);
    }
}

void MusicList::LoadFile(const path pathSong) {
    if(IsSupported(pathSong) && !is_directory(pathSong)) {

        spdlog::get("global")->trace("Loaded {}", pathSong.c_str());

        auto song = std::make_shared<Song>(pathSong);
        full_list.emplace_back(song);
        song_list.emplace_back(song);
    }
}

void MusicList::LoadPlaylist(std::string pathPl) {
    std::ifstream pl(pathPl);
    if(!pl.is_open()) throw std::runtime_error("Can not open playlist");

    song_list.clear();
    for(std::string l; std::getline(pl, l);) {
        LoadFile(path(l)); // This is clean safe code ???
    }
}

void MusicList::SavePlaylist(std::string pathPl) {
    std::ofstream f(pathPl);
    if(!f.is_open()) throw std::runtime_error("Can not open playlist");

    for(auto& s : GetSongList()) f << s->GetFile() << std::endl;
}

void MusicList::SaveArtists(std::string pathPl) {
    std::ofstream f(pathPl);
    if(!f.is_open()) throw std::runtime_error("Can not open playlist");

    std::set<std::string> artists_set;
    for(auto& a : GetSongList()) artists_set.emplace(a->GetArtist());

    for(auto& a : artists_set) f << a << std::endl;
}

void MusicList::Sort(Order s) {
    switch(s) {
    case Order::RANDOM:
        SortRandom();
        break;
    case Order::LLF:
        SortLLF();
        break;
    case Order::MLF:
        SortMLF();
        break;
    }
}

void MusicList::FilterArtist(const std::string& artist) {
    if(artist != "") {
        spdlog::get("global")->trace("Filter request {}", artist);
        song_list.clear();
        for(auto s : full_list) {
            if(s->GetArtist() == artist) {
                spdlog::get("global")->trace("Analyzing; {}; {}",
                                             s->GetArtist(), s->GetFile());
                song_list.emplace_back(s);
            }
        }
    } else {
        song_list = full_list;
    }
}

std::vector<std::shared_ptr<Song>>& MusicList::GetSongList() {
    return song_list;
}

// Private functions

bool MusicList::IsSupported(path p) {
    static const std::array<path, 4> formats = {path(".mp3"), path(".flac"),
                                                path(".ogg"), path(".wav")};
    for(auto& v : formats)
        if(p.extension() == v) return true;

    return false;
}

void MusicList::SortRandom() {
    std::random_device rd;
    std::shuffle(song_list.begin(), song_list.end(), std::mt19937(rd()));
}

void MusicList::SortLLF() {
    auto cmp = [](std::shared_ptr<Song> a, std::shared_ptr<Song> b) {
        return a->GetReproductions() < b->GetReproductions();
    };

    std::sort(song_list.begin(), song_list.end(), cmp);
}

void MusicList::SortMLF() {
    auto cmp = [](std::shared_ptr<Song> a, std::shared_ptr<Song> b) {
        return a->GetReproductions() > b->GetReproductions();
    };

    std::sort(song_list.begin(), song_list.end(), cmp);
}
