#pragma once

#include "song.hpp"


#include <boost/filesystem.hpp>
#include <vector>
#include <string>
#include <memory>

using namespace boost::filesystem;

enum class Order {
	RANDOM,
	LLF,
	MLF,
};

class MusicList {
public:
	//Search music recursively in the "dir" path and add it to the db
	void LoadDir(const path& dir);

	void LoadFile(const path& pathSong);

	void LoadPlaylist(std::string pathPl);

	void SavePlaylist(std::string pathPl);

	void SaveArtists(std::string pathPl);


	void Sort(Order s);

	void FilterArtist(const std::string& artist);

	std::vector<std::shared_ptr<Song>>& GetSongList();
private:
	bool IsSupported(path p);

	void SortRandom();
	void SortLLF();
	void SortMLF();


	//List of songs that will be reproduced
	std::vector<std::shared_ptr<Song>> song_list;

	//List with all songs;
	std::vector<std::shared_ptr<Song>> full_list;
};