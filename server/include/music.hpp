#pragma once

#include <string>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include <boost/filesystem.hpp>
#include <SFML/Audio.hpp>

#include "musiclist.hpp"
#include "sound.hpp"
#include "song.hpp"

using namespace boost::filesystem;

//Possible states
enum class Status {
    Playing,
    Paused,
    Stoped,
    Exit,

    Forwarding,
    Backing,
    Restart,
};

//This class implements the basic music class, that is, without playing anything
class Music {
public:
    Music() =default;
    void SetVolume(float v);
    float GetVolume();

    int GetRemainingMilliseconds();

    void SetPlayingOffset(int ms);
    void PlayList();

    Status GetStatus() const;
    void SetStatus(Status s);

    MusicList& GetList();
    Song& GetCurrent();

private:
    void Reproduce();
    bool IsStatus(Status s);
    bool IsNotStatus(Status s);

    bool OpenCurrent();

    Sound music;
    Song song;

    std::atomic<Status> status {Status::Stoped};
    MusicList list;

    std::atomic<bool> is_playing {false};
    std::atomic<bool> status_processed {true};
    std::condition_variable cv;
    std::condition_variable status_cv;
};
