//
// Created by franciszek on 23.01.2020.
//

#ifndef PLAYLIST_EXCEPTIONS_H
#define PLAYLIST_EXCEPTIONS_H

#include <exception>

class PlayerException : public std::exception{};

class CorruptedFileException : public PlayerException {
	[[nodiscard]] const char* what() const noexcept override;
};

class CorruptedFileContentException : public PlayerException {
	[[nodiscard]] const char* what() const noexcept override;
};

class UnsupportedFormatException : public PlayerException {
	[[nodiscard]] const char* what() const noexcept override;
};

class IndexOutOfBoundsException : public PlayerException {
	[[nodiscard]] const char* what() const noexcept override;
};

class AttemptedCycleCreationException : public PlayerException {
	[[nodiscard]] const char* what() const noexcept override;
};




#endif //PLAYLIST_EXCEPTIONS_H
