//
// Created by franciszek on 11.01.2020.
//

#ifndef PLAYLIST_PLAYLIST_H
#define PLAYLIST_PLAYLIST_H

#include <cstddef>
#include <iostream>
#include "element.h"
#include "mode.h"


class PlayList : public Element, public std::enable_shared_from_this<Element> {
public:
	void play() override;
	void add(const std::shared_ptr<Element>& element);
	void add(const std::shared_ptr<Element>& element, size_t position);
	void remove();
	void remove(size_t position);
	void setMode(std::shared_ptr<Mode> newMode);
	bool canBeAdded(const std::shared_ptr<Element>& element) override;
	explicit PlayList(std::string name);
	~PlayList() override = default;
private:
	std::deque<std::shared_ptr<Element>> elements;
	std::shared_ptr<Mode> mode;
	std::string name;
};

#endif //PLAYLIST_PLAYLIST_H
