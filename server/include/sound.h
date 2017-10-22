#pragma once

#include <SFML/Audio.hpp>
#include "mpg123.h"

enum class Format : char {
    MP3,
    OTHER,
    /*
    OGG,
    FLAC,
    WAV,*/
};


class Mp3 : public sf::Music {
public:
    Mp3();
    ~Mp3();

    sf::Time getDuration() const;

    bool openFromFile(const std::string& filename);

protected:
    bool onGetData(Chunk& data);
    void onSeek(sf::Time timeOffset);

private:
    sf::Time            myDuration;
    mpg123_handle*      myHandle;
    size_t              myBufferSize;
    unsigned char*      myBuffer;
    sf::Mutex           myMutex;
};


class Sound : public Mp3
{
public:
    Sound() =default;
    sf::Time getDuration() const;

    bool openFromFile(const std::string& filename, Format);

protected:
    bool onGetData(Chunk& data);
    void onSeek(sf::Time timeOffset);

private:
    Format              format;
};