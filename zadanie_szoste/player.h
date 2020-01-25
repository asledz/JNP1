#ifndef PLAYLIST_PLAYER_H
#define PLAYLIST_PLAYER_H

#include "media.h"
#include "playlist.h"

class Player {
public:
	static std::shared_ptr<PlayList> createPlaylist(const char *name);
	
	static std::shared_ptr<Element> openFile(File file);
};

#endif //PLAYLIST_PLAYER_H
