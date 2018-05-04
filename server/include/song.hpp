#pragma once

/// @file song.h
/// @brief Declaration of Song class

#include <string>
#include <boost/filesystem.hpp>

using namespace boost::filesystem;

/// @class Song
/// @brief Class to retrieve metadata
class Song {
public:
	/// @brief Creates a Song of an absolute path
	explicit Song(const path& p);

	/// @brief Creates a temporary empty Song.
	Song() =default;

	/// @brief Retrieve the title of this Song.
	/// @return The title if there are no errors or "Unknown" otherwise
	std::string GetTitle();

	/// @brief Retrieve the artist name of this Song.
	/// @return The title if there are no errors or "Unknown" otherwise
	std::string GetArtist();

	/// @brief Retrieve the album name of this Song.
	/// @return The title if there are no errors or "Unknown" otherwise
	std::string GetAlbum();

	/// @brief Retrieve the extension of the file, for example ".mp3"
	/// @return The extension
	std::string GetExtension();

	/// @brief Retrieve the absolute path to the file.
	/// @return The absoulte path
	std::string GetFile();

	unsigned GetReproductions();

	void SetReproductions(unsigned i);

private:
    void GetTitleFromFile();
    void GetArtistFromFile();
    
	std::string album;
	std::string extension;
	std::string artist;
	std::string title;
	std::string file;
	unsigned reproductions {0};
};
