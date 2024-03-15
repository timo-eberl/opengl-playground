#include "texture.h"

#include <cassert>

#include <stb_image.h>

#include "log.h"

#define ASSETS_DIR _ASSETS_DIR

using ron::Texture;

Texture::Texture(
	const ImageData image_data, const std::string& name,
	const MetaData meta_data, const SampleData sample_data
) : name(name), image_data(image_data), meta_data(meta_data), sample_data(sample_data)
{}

Texture::ImageData Texture::image_data_from_memory(
	const unsigned char *memory, const int len
) {
	assert(memory);

	ImageData image_data = {};

	image_data.data_ptr = stbi_load_from_memory(
		memory, len, &image_data.width, &image_data.height, &image_data.n_channels, 0
	);
	if (!image_data.data_ptr) {
		log::error("Failed to load texture from memory");
		stbi_image_free(image_data.data_ptr);
	}

	return image_data;
}

Texture::ImageData Texture::image_data_from_file(
	const std::string &absolute_path
) {
	ImageData image_data = {};

	image_data.data_ptr = stbi_load(
		absolute_path.c_str(), &image_data.width, &image_data.height, &image_data.n_channels, 0
	);
	if (!image_data.data_ptr) {
		log::error("Failed to load texture \"" + absolute_path + "\"");
		stbi_image_free(image_data.data_ptr);
	}

	return image_data;
}

Texture::~Texture() { if (good()) stbi_image_free(image_data.data_ptr); }

bool Texture::good() const { return image_data.data_ptr; }

unsigned int Texture::get_update_count() const { return m_update_count; }

void Texture::update(const ImageData image_data) {
	if (good()) {
		stbi_image_free(image_data.data_ptr);
	}
	this->image_data = image_data;
	m_update_count++;
}

void Texture::mark_as_updated() { m_update_count++; }
