#include "texture.h"

#include <stb_image.h>

#include "log.h"

#define ASSETS_DIR _ASSETS_DIR

using glpg::Texture;

Texture::Texture(
	const std::string& asset_path, const Format format, const WrapMode wrap_mode,
	const MinifyingFilter min_filter, const MagnificationFilter mag_filter
)
	: asset_path(asset_path), format(format),
	wrap_mode(wrap_mode), min_filter(min_filter), mag_filter(mag_filter)
{
	load();
}

Texture::~Texture() { stbi_image_free(data); }

bool Texture::good() const { return data; }

void Texture::reload_from_file() { stbi_image_free(data); load(); }

void Texture::load() {
	stbi_set_flip_vertically_on_load(true);
	const std::string absolute_path = ASSETS_DIR + asset_path;
	data = stbi_load(absolute_path.c_str(), &width, &height, &n_channels, 0);
	if (!data) {
		log::error("Failed to load texture \"" + asset_path + "\"");
		stbi_image_free(data);
	}
}
