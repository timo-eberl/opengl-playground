#include "assets.h"

#include <fstream>
#include <sstream>
#include <cstring>

#include "log.h"

#define ASSETS_DIR _ASSETS_DIR

std::string glpg::assets::read_text_file(const std::string& asset_path) {
	const auto complete_path = ASSETS_DIR + asset_path;

	std::string file_content;

	std::ifstream ifstream;
	ifstream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		ifstream.open(complete_path);

		std::stringstream ss;
		ss << ifstream.rdbuf();
		file_content = ss.str();

		ifstream.close();
	} catch (std::ifstream::failure e) {
		log::error(
			std::string("Exception opening/reading/closing file \"")
			+ complete_path + "\": " + std::strerror(errno)
		);
	}

	return file_content;
}
