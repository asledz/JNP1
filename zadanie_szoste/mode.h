//
// Created by franciszek on 12.01.2020.
//

#ifndef PLAYLIST_BASEMODE_H
#define PLAYLIST_BASEMODE_H

#include <deque>
#include <memory>
class Element;


// TODO tutaj na pewno przyda się Strategy i Factory Patterns
class Mode {
public:
	virtual void play(std::deque<std::shared_ptr<Element>> const &playList) = 0;
	Mode() = default;
	virtual ~Mode() = default;
};

#endif //PLAYLIST_BASEMODE_H
