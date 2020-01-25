#ifndef PLAYLIST_ELEMENT_H
#define PLAYLIST_ELEMENT_H


#include <memory>

class Element {
public:
	virtual void play() = 0;
	virtual bool canBeAdded(const std::shared_ptr<Element> &element) = 0;
	Element() = default;
	virtual ~Element() = default;
};



#endif //PLAYLIST_ELEMENT_H
