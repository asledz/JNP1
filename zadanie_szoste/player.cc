#include "player.h"
#include "exceptions.h"

#include<regex>

std::shared_ptr<PlayList> Player::createPlaylist(const char *name) {
	return std::make_shared<PlayList>(name);
}


std::shared_ptr<Element> Player::openFile(File file) {
	std::string fileContent = file.getContent();
	std::string type = file.getType();

	if (type == "audio") {
		return std::static_pointer_cast<Element>(std::make_shared<Audio>(file.getMetadata(), file.getContent()));
	} else if (type == "video") {
		return std::static_pointer_cast<Element>(std::make_shared<Video>(file.getMetadata(), file.getContent()));
	} else {
		throw UnsupportedFormatException();
	}
}


