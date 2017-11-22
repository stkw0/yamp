#include "sound.hpp"
#include <iostream>

// Mp3

Mp3::Mp3() : myHandle(nullptr), myBufferSize(0), myBuffer(nullptr) {
    int err = MPG123_OK;
    if((err = mpg123_init()) != MPG123_OK) {
        std::cerr << mpg123_plain_strerror(err) << std::endl;
        return;
    }

    myHandle = mpg123_new(NULL, &err);
    if(!myHandle) {
        std::cerr << "Unable to create mpg123 handle: " << mpg123_plain_strerror(err)
                  << std::endl;
        return;
    }
}

Mp3::~Mp3() {
    stop();

    if(myBuffer) {
        delete[] myBuffer;
        myBuffer = NULL;
    }

    mpg123_close(myHandle);
    mpg123_delete(myHandle);
    mpg123_exit();
}

sf::Time Mp3::getDuration() const {
    return myDuration;
}

bool Mp3::openFromFile(const std::string& filename) {

    stop();

    if(myBuffer) delete[] myBuffer;

    if(myHandle) mpg123_close(myHandle);

    mpg123_param(myHandle, MPG123_RESYNC_LIMIT, -1, 0);
#ifndef DEBUG
    mpg123_param(myHandle, MPG123_ADD_FLAGS, 32, 0);
#endif

    if(mpg123_open(myHandle, filename.c_str()) != MPG123_OK) {
        std::cerr << mpg123_strerror(myHandle) << std::endl;
        return false;
    }

    // This should improve myDuration calculation, but generates frankenstein
    // streams¿?  Warning: Real sample count 9505152 differs from given gapless
    // sample count -1152. Frankenstein stream
    if(mpg123_scan(myHandle) != MPG123_OK) {
        std::cerr << "Failed when scanning " << std::endl;
        return false;
    }

    long rate = 0;
    int channels = 0, encoding = 0;
    if(mpg123_getformat(myHandle, &rate, &channels, &encoding) != MPG123_OK) {
        std::cerr << "Failed to get format information for "
                     "464480e9ee6eb73bc2b768d7e3d7865aa432fc34quot;"
                  << filename << "464480e9ee6eb73bc2b768d7e3d7865aa432fc34quot;"
                  << std::endl;
        return false;
    }

    myDuration = sf::Time(sf::milliseconds(1 + 1000 * mpg123_length(myHandle) / rate));

    myBufferSize = mpg123_outblock(myHandle);
    myBuffer = new unsigned char[myBufferSize];
    if(!myBuffer) {
        std::cerr << "Failed to reserve memory for decoding one frame for "
                     "464480e9ee6eb73bc2b768d7e3d7865aa432fc34quot;"
                  << filename << "464480e9ee6eb73bc2b768d7e3d7865aa432fc34quot;"
                  << std::endl;
        return false;
    }

    sf::SoundStream::initialize(channels, rate);

    return true;
}

bool Mp3::onGetData(Chunk& data) {

    sf::Lock lock(myMutex);

    if(myHandle) {
        size_t done;
        mpg123_read(myHandle, myBuffer, myBufferSize, &done);

        data.samples = (short*)myBuffer;
        data.sampleCount = done / sizeof(short);

        return (data.sampleCount > 0);
    } else
        return false;
}

void Mp3::onSeek(sf::Time timeOffset) {
    sf::Lock lock(myMutex);

    if(myHandle)
        mpg123_seek_frame(myHandle, mpg123_timeframe(myHandle, timeOffset.asSeconds()), SEEK_SET);
}

// End Mp3

// Sound

bool Sound::openFromFile(const std::string& filename, Format f) {
    format = f;

    if(format != Format::MP3)
        return sf::Music::openFromFile(filename);
    else
        return Mp3::openFromFile(filename);
}

bool Sound::onGetData(Chunk& data) {
    if(format != Format::MP3)
        return sf::Music::onGetData(data);
    else
        return Mp3::onGetData(data);
}

void Sound::onSeek(sf::Time timeOffset) {
    if(format != Format::MP3)
        return sf::Music::onSeek(timeOffset);
    else
        return Mp3::onSeek(timeOffset);
}

sf::Time Sound::getDuration() const {
    if(format != Format::MP3)
        return sf::Music::getDuration();
    else
        return Mp3::getDuration();
}
