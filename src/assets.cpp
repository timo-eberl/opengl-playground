#include "assets.h"

#include <fstream>
#include <sstream>
#include <cstring>
#include <vector>
#include <map>

#include "log.h"

#define ASSETS_DIR _ASSETS_DIR

using namespace ron;

std::string assets::read_text_file(const std::string& asset_path) {
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

// keep track of all loaded shaders
static auto loaded_shaders = std::map<
	std::pair<std::string, std::string>,
	std::weak_ptr<ShaderProgram>
>();

std::shared_ptr<ShaderProgram> assets::load_shader_program(
	const std::string& vertex_shader_asset_path, const std::string& fragment_shader_asset_path
) {
	const auto asset_path_pair = std::make_pair(vertex_shader_asset_path, fragment_shader_asset_path);

	// if shader is already loaded, update and return it
	if (loaded_shaders.contains(asset_path_pair)) {
		const auto existing_shader_program = loaded_shaders[asset_path_pair];
		if (const auto sp_existing = existing_shader_program.lock()) {
			// the update will only happen, if the content changed
			sp_existing->update(
				assets::read_text_file(vertex_shader_asset_path),
				assets::read_text_file(fragment_shader_asset_path)
			);
			return sp_existing;
		}
	}

	const auto shader_program = std::make_shared<ShaderProgram>(
		assets::read_text_file(vertex_shader_asset_path),
		assets::read_text_file(fragment_shader_asset_path),
		vertex_shader_asset_path + ", " + fragment_shader_asset_path
	);

	loaded_shaders[asset_path_pair] = shader_program;

	return shader_program;
}

void assets::reload_shader_programs() {
	for (const auto &[asset_paths, shader_program] : loaded_shaders) {
		if (const auto sp_shader_program = shader_program.lock()) {
			// the update will only happen, if the content changed
			sp_shader_program->update(
				assets::read_text_file(asset_paths.first),
				assets::read_text_file(asset_paths.second)
			);
		}
	}
}
