#include "modes.h"
#include "element.h"
#include <algorithm>


// Modes (from modes.cc)
std::shared_ptr<Mode> createOddEvenMode() {
	static std::shared_ptr<OddEvenMode> instance = std::make_shared<OddEvenMode>();
	return instance;
}

std::shared_ptr<Mode> createShuffleMode(std::default_random_engine::result_type seed) {
	return std::make_shared<ShuffleMode>(seed);
}

std::shared_ptr<Mode> createSequenceMode() {
	static std::shared_ptr<SequenceMode> instance = std::make_shared<SequenceMode>();
	return instance;
}

// OddEvenMode (from oddevenmode.cc)
void OddEvenMode::play(std::deque<std::shared_ptr<Element>> const &playList) {
	size_t n = playList.size();

	for (size_t i = 0; i < n; i += 2) {
		playList[i]->play();
	}

	for (size_t i = 1; i < n; i += 2) {
		playList[i]->play();
	}
}

// SequenceMode (from sequencemode.cc)

void SequenceMode::play(std::deque<std::shared_ptr<Element>> const &playList) {
	for (const std::shared_ptr<Element>& element : playList) {
		element->play();
	}
}

// ShuffleMode (from shufflemode.cc)

void ShuffleMode::play(std::deque<std::shared_ptr<Element>> const &playList) {
	std::vector<size_t> indexes(playList.size());
	std::iota(indexes.begin(), indexes.end(), 0);
	std::shuffle(indexes.begin(), indexes.end(), engine);
	for (size_t index : indexes) {
		playList[index]->play();
	}
}