#ifndef PLAYLIST_MEDIA_H
#define PLAYLIST_MEDIA_H

#include <string>
#include <unordered_map>
#include "element.h"


using Metadata = std::unordered_map<std::string, std::string>;
// Media
class Media : public Element {
public:
	bool canBeAdded(const std::shared_ptr<Element> &element) override;
	~Media() override = default;
};

// Playlist w osobnym pliku

// Audio

class Audio : public Media {
public:
	explicit Audio(Metadata& metadata, std::string file_content);
	~Audio() override = default;
	void play() override;
private:
	std::string title;
	std::string artist;
	std::string content;
};

// Video

class Video : public Media {
public:
	explicit Video(Metadata& metadata, std::string file_content);
	~Video() override = default;
	void play() override;
private:
	std::string title;
	std::string content;
	int year;
};


// File

class File {
public:
	explicit File(const char *content);
	std::string getContent();
	std::string getType();
	Metadata& getMetadata();

private:
	std::string type;
	std::string file_contents;
	Metadata metadata;
};


#endif //PLAYLIST_MEDIA_H

