//
// Created by franciszek on 23.01.2020.
//

#include "exceptions.h"

const char* CorruptedFileException::what() const noexcept {
	return "Cannot create file due to corrupted data";
}

const char* CorruptedFileContentException::what() const noexcept {
	return "Cannot open file due to corrupted file contents";
}

const char* UnsupportedFormatException::what() const noexcept {
	return "Cannot parse file due to unsupported format type";
}

const char *IndexOutOfBoundsException::what() const noexcept {
	return "Index out of bounds";
}

const char *AttemptedCycleCreationException::what() const noexcept {
	return "Cannot add element, because doing so would create cyclic relation";
}
