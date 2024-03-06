#include "log.h"

#include <iostream>

using namespace glpg;

void log::info(const std::string& text) {
	std::cout << text << std::endl;
}

void log::warn(const std::string& text, bool colored) {
	if (colored) {
		std::cout
			<< "\033[1;33m" // font color yellow
			<< text
			<< "\033[1;0m" // reset styling
			<< std::endl;
	}
	else {
		std::cout
			<< "\033[1;1m" // bold
			<< text
			<< "\033[1;22m" // undo bold
			<< std::endl;
	}
}

void log::error(const std::string& text, bool colored) {
	if (colored) {
		std::cerr
			<< "\033[1;31m" // font color red
			<< text
			<< "\033[1;0m" // reset styling
			<< std::endl;
	}
	else {
		std::cerr
			<< "\033[1;1m" // bold
			<< text
			<< "\033[1;22m" // undo bold
			<< std::endl;
	}
}

void log::success(const std::string& text, bool colored) {
	if (colored) {
		std::cout
			<< "\033[1;32m" // font color green
			<< text
			<< "\033[1;0m" // reset styling
			<< std::endl;
	}
	else {
		std::cout
			<< "\033[1;1m" // bold
			<< text
			<< "\033[1;22m" // undo bold
			<< std::endl;
	}
}
