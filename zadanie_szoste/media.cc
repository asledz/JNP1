#include "media.h"
#include "exceptions.h"
#include <regex>
#include <string>
#include <iostream>

// UTIL FUNCTIONS DECLARATION
std::string first_match(std::string &text, std::regex &r);

void rot13(std::string &text);


// Media
bool Media::canBeAdded(const std::shared_ptr<Element> &element) {
	return true;
}

// Audio

Audio::Audio(Metadata& metadata, std::string file_content) {
	std::regex content_regex("[,.!?':;\\-\\.a-zA-z\\s]*");
	auto it_title = metadata.find("title");
	auto it_artist = metadata.find("artist");
	if (it_artist == metadata.end() || it_title == metadata.end() || !std::regex_match(file_content, content_regex)) {
		throw CorruptedFileContentException();
	}

	this->content = file_content;
	this->artist = it_artist->second;
	this->title = it_title->second;
}

void Audio::play() {
	std::cout << "Song [" << artist << ", " << title << "]:\n" << content << std::endl;
}

// Video

Video::Video(Metadata& metadata, std::string file_content) {
	std::regex content_regex("[,.!?':;\\-\\.a-zA-z\\s]*");
	auto it_title = metadata.find("title");
	auto it_year = metadata.find("year");
	if (it_year == metadata.end() || it_title == metadata.end() || !std::regex_match(file_content, content_regex)) {
		throw CorruptedFileContentException();
	}


	rot13(file_content);

	this->content = file_content;
	this->title = it_title->second;
	try {
		this->year = std::stoi(it_year->second);
	} catch (std::invalid_argument &e) {
		throw CorruptedFileContentException();
	}
}

void Video::play() {
	std::cout << "Movie [" << title << ", " << year << "]:\n" << content << std::endl;
}

// File

File::File(const char* content) {
	std::string helper(content);
	std::regex file_regex ("[\\w]*(\\|[^:\\n\\|]*:[^:\\n\\|]*)*\\|.*");
	std::regex part_regex ("[\\w]*:[^\\|]*");
	std::vector<std::string> parts;
	if(!std::regex_match(content, file_regex)) {
		throw CorruptedFileException();
	}

	std::istringstream ss(helper);
	std::string token;

	while (std::getline(ss, token, '|')) {
		if (token.length() > 0)
			parts.push_back(token);
	}

	auto it = parts.begin();

	type = *it;
	it++;

	while (it != parts.end() && std::regex_match(*it, part_regex)) {
		size_t delim_pos = (*it).find(':');
		std::string key = (*it).substr(0, delim_pos);
		std::string value = (*it).substr(delim_pos + 1);
		metadata.insert({key, value});
		it++;
	}

	std::string new_file_contents;

	while (it != parts.end()) {
		new_file_contents += *it;
		it++;
	}

	file_contents = new_file_contents;
}

std::string File::getContent() {
	return file_contents;
}

std::string File::getType() {
	return type;
}

std::unordered_map<std::string, std::string>& File::getMetadata() {
	return metadata;
}

/* UTIL FUNCTIONS */

std::string first_match(std::string &text, std::regex &r) {
    std::smatch match;
    std::regex_search(text, match, r);
    std::string res = match.str(0);
    return res;
}

void rot13(std::string &text) {
	for(char & i : text) {
		int a = i;
		if(a >= 'a' && a  <= 'z') {
			a = (a - 'a' - 13 + 26)%26 + 'a';
			i = char(a);
		}
		else if(a >= 'A' && a <= 'Z') {
			a = (a - 'A' - 13 + 26) %26 + 'A';
			i = char(a);
		}
	}
}