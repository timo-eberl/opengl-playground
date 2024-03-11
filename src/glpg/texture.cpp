#include "texture.h"

#include <cassert>

#include <stb_image.h>

#include "log.h"

#define ASSETS_DIR _ASSETS_DIR

using glpg::Texture;

Texture::Texture(
	const std::string& asset_path,
	const MetaData meta_data, const SampleData sample_data
) : has_file(true), asset_path(asset_path), name(asset_path),
	meta_data(meta_data), sample_data(sample_data)
{
	load_from_file();
}

Texture::Texture(
	const ImageData image_data, const std::string& name,
	const MetaData meta_data, const SampleData sample_data
) : has_file(false), asset_path(""), name(name),
	image_data(image_data), meta_data(meta_data), sample_data(sample_data)
{}

Texture::ImageData Texture::image_data_from_memory(
	const unsigned char *memory, const int len, const bool flip
) {
	assert(memory);

	ImageData image_data = {};
	stbi_set_flip_vertically_on_load(flip);
	image_data.data_ptr = stbi_load_from_memory(
		memory, len, &image_data.width, &image_data.height, &image_data.n_channels, 0
	);
	if (!image_data.data_ptr) {
		log::error("Failed to load texture from memory");
		stbi_image_free(image_data.data_ptr);
	}

	return image_data;
}

Texture::~Texture() { if (good()) stbi_image_free(image_data.data_ptr); }

bool Texture::good() const { return image_data.data_ptr; }

unsigned int Texture::get_update_count() const { return m_update_count; }

void Texture::reload_from_file() {
	if (has_file) {
		stbi_image_free(image_data.data_ptr);
		load_from_file();
		m_update_count++;
	}
}

void Texture::load_from_file() {
	stbi_set_flip_vertically_on_load(meta_data.flip_y);
	const std::string absolute_path = ASSETS_DIR + asset_path;
	image_data.data_ptr = stbi_load(
		absolute_path.c_str(), &image_data.width, &image_data.height, &image_data.n_channels, 0
	);
	if (!image_data.data_ptr) {
		log::error("Failed to load texture \"" + asset_path + "\"");
		stbi_image_free(image_data.data_ptr);
	}
}
