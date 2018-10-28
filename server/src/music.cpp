#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

#include <SFML/Audio.hpp>
#include <spdlog/spdlog.h>

#include "music.hpp"
#include "utils.hpp"

static std::mutex status_mutex;

// Public functions

void Music::PlayList() {
    while(GetStatus() != Status::Exit) {
        // Wait here until state is not Stoped
        std::mutex cv_m;
        std::unique_lock<std::mutex> lk{cv_m};
        cv.wait(lk, [this] { return IsNotStatus(Status::Stoped); });

        auto& musicList = list.GetSongList();
        for(auto s = musicList.begin(); s != musicList.end(); ++s) {
            if(IsStatus(Status::Exit) || IsStatus(Status::Stoped)) break;

            IsStatus(Status::Forwarding);

            if(IsStatus(Status::Playing)) is_playing = true;

            song = **s;
            if(OpenCurrent()) {
                ExecuteScript("play.hook");
                if(is_playing) {
                    status = Status::Playing;
                } else {
                    status = Status::Paused;
                }

                Reproduce();
            }

            if(IsStatus(Status::Backing)) {
                if(s - 1 >= musicList.begin()) {
                    s -= 2;
                } else {
                    s -= 1;
                }
                SetStatus(Status::Playing);
            }

            spdlog::get("global")->trace("{}", (int)GetStatus());
            if(IsStatus(Status::Stoped)) break;

            spdlog::get("global")->trace("Next song");
        }

        if(GetStatus() != Status::Exit) SetStatus(Status::Stoped);
        spdlog::get("global")->trace("Loop ended");
    }
}

Status Music::GetStatus() const {
    std::lock_guard<std::mutex> lock(status_mutex);
    return status;
}

void Music::SetStatus(Status s) {
    // Wait previous status to be processed
    std::mutex cv_m;
    std::unique_lock<std::mutex> lk{cv_m};
    status_cv.wait(lk, [this] { return status_processed.load(); });

    // Fix bug: when setting two times the same status it stops reading
    if(status == s) return;

    status_processed = false;

    status = s;

    cv.notify_one();
}

void Music::SetVolume(float v) {
    music.setVolume(v);
}

float Music::GetVolume() {
    return music.getVolume();
}

int Music::GetRemainingMilliseconds() {
    return music.getDuration().asMilliseconds() - music.getPlayingOffset().asMilliseconds();
}

void Music::SetPlayingOffset(int ms) {
    // ms: Millisecond in the song to move the current offset

    if(ms < 0) {
        ms = 0;
    } else {
        auto duration = music.getDuration().asMilliseconds();
        if(ms > duration) ms = duration;
    }

    music.setPlayingOffset(sf::milliseconds(ms));

    // This is to update the sleep_time
    SetStatus(Status::Paused);
    SetStatus(Status::Playing);
}

bool Music::IsStatus(Status s) {
    bool tmp = GetStatus() == s;
    if(tmp) {
        status_processed = true;
        status_cv.notify_one();
    }
    return tmp;
}

bool Music::IsNotStatus(Status s) {
    bool tmp = GetStatus() != s;
    if(!tmp) {
        status_processed = true;
        status_cv.notify_one();
    }
    return tmp;
}

MusicList& Music::GetList() {
    return list;
}

Song& Music::GetCurrent() {
    return song;
}

// Private functions

bool Music::OpenCurrent() {
    Format f = Format::OTHER;

    if(song.GetExtension() == ".mp3") f = Format::MP3;

    if(!music.openFromFile(song.GetFile(), f)) {
        spdlog::get("global")->error("Can not open file", song.GetFile());
        std::rename(song.GetFile().c_str(),
                    (song.GetFile() + ".not_reproducible").c_str());
        return false;
    }

    return true;
}

void Music::Reproduce() {
    spdlog::get("global")->info("Playing: {}", song.GetFile());

    auto duration = music.getDuration().asMilliseconds();

    music.play();

    std::mutex cv_m;
    std::unique_lock<std::mutex> lk{cv_m};

    bool loop = true;
    // loop only if the command is Pause
    while(loop) {
        loop = false;

        // Calculate how many milliseconds we have to sleep for finish the song
        auto offset = music.getPlayingOffset().asMilliseconds();
        auto sleep_time = duration - offset;

        if(sleep_time < 0) {
            // It should never happen
            throw std::logic_error(
                "Playing offset is greater than total length");
        }

        spdlog::get("global")->debug("Sleeping {} milliseconds", sleep_time);

        // Wait until we have something to do or until the song finish
        cv.wait_for(lk, std::chrono::milliseconds(sleep_time),
                    [this] { return IsNotStatus(Status::Playing); });

        if(IsStatus(Status::Paused)) {

            music.pause();
            cv.wait(lk, [this] { return IsNotStatus(Status::Paused); });
            if(IsNotStatus(Status::Playing))
                is_playing = false;
            else
                is_playing = true;

            music.play();
            loop = true;
        } else if(IsStatus(Status::Restart)) {
            is_playing = true;
            music.setPlayingOffset(sf::seconds(0));
            status = Status::Playing;
        } else if(IsStatus(Status::Playing)) {
            is_playing = true;
            // Only increment if we have listened the full song ¿?
            song.SetReproductions(song.GetReproductions() + 1);
        }
    }
    music.stop();
}
