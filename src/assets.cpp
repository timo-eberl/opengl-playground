#include "assets.h"

#include <fstream>
#include <sstream>
#include <cstring>
#include <vector>
#include <map>
#include <filesystem>
#include <cassert>

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
	} catch (std::ifstream::failure &e) {
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
		const auto wp_existing = loaded_shaders[asset_path_pair];
		if (const auto sp_existing = wp_existing.lock()) {
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

struct TextureWithLastUpdateTime {
	std::weak_ptr<Texture> texture;
	std::filesystem::file_time_type last_update_time;
};

struct TextureIdentifier {
	std::string asset_path;
	Texture::MetaData meta_data;
	Texture::SampleData sample_data;

	// implement < so it can be used as a key in std::map
	bool operator<(const TextureIdentifier& rhs) const {
		if (asset_path != rhs.asset_path) { return asset_path < rhs.asset_path; }
		if (meta_data.channels != rhs.meta_data.channels) { return meta_data.channels < rhs.meta_data.channels; }
		if (meta_data.color_space != rhs.meta_data.color_space) { return meta_data.color_space < rhs.meta_data.color_space; }
		if (sample_data.mag_filter != rhs.sample_data.mag_filter) { return sample_data.mag_filter < rhs.sample_data.mag_filter; }
		if (sample_data.min_filter != rhs.sample_data.min_filter) { return sample_data.min_filter < rhs.sample_data.min_filter; }
		if (sample_data.wrap_mode_s != rhs.sample_data.wrap_mode_s) { return sample_data.wrap_mode_s < rhs.sample_data.wrap_mode_s; }
		return sample_data.wrap_mode_t < rhs.sample_data.wrap_mode_t;
	}
};

// keep track of all loaded textures
static auto loaded_textures = std::map<TextureIdentifier,TextureWithLastUpdateTime> ();

std::shared_ptr<Texture> assets::load_texture(
	const std::string &asset_path,
	const Texture::MetaData &meta_data,
	const Texture::SampleData &sample_data
) {
	const auto complete_path = ASSETS_DIR + asset_path;

	const auto texture_identifier = TextureIdentifier(asset_path, meta_data, sample_data);

	// if texture is already loaded, update and return it
	if (loaded_textures.contains(texture_identifier)) {
		auto &existing = loaded_textures[texture_identifier];
		if (const auto sp_existing = existing.texture.lock()) {
			// update only, if the file has been modified
			const auto last_write_time = std::filesystem::last_write_time(complete_path);
			if (last_write_time > existing.last_update_time) {
				sp_existing->update(Texture::image_data_from_file(complete_path));
				existing.last_update_time = last_write_time;
			}

			assert(sp_existing->meta_data.channels == meta_data.channels);
			assert(sp_existing->meta_data.color_space == meta_data.color_space);
			assert(sp_existing->sample_data.mag_filter == sample_data.mag_filter);
			assert(sp_existing->sample_data.min_filter == sample_data.min_filter);
			assert(sp_existing->sample_data.wrap_mode_s == sample_data.wrap_mode_s);
			assert(sp_existing->sample_data.wrap_mode_t == sample_data.wrap_mode_t);
			assert(sp_existing->sample_data.wrap_mode_t == sample_data.wrap_mode_t);

			return sp_existing;
		}
	}

	const auto texture = std::make_shared<Texture>(
		Texture::image_data_from_file(complete_path), asset_path, meta_data, sample_data
	);

	loaded_textures[texture_identifier] = TextureWithLastUpdateTime(
		texture,
		std::filesystem::last_write_time(complete_path)
	);

	return texture;
}

void assets::reload_textures() {
	for (auto &[texture_identifier, texture_with_last_update_time] : loaded_textures) {
		const auto complete_path = ASSETS_DIR + texture_identifier.asset_path;

		if (const auto sp_texture = texture_with_last_update_time.texture.lock()) {
			// update only, if the file has been modified
			const auto last_write_time = std::filesystem::last_write_time(complete_path);
			if (last_write_time > texture_with_last_update_time.last_update_time) {
				sp_texture->update(Texture::image_data_from_file(complete_path));
				texture_with_last_update_time.last_update_time = last_write_time;
			}
		}
	}
}
