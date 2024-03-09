#include "texture.h"

#include <cassert>

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

const std::shared_ptr<Texture> Texture::get_fallback_texture(const FallbackColor fallback_color) {
	static const std::shared_ptr<Texture> static_white_texture = std::make_shared<Texture>(
		"textures/fallback/white.jpg", Texture::Format::RGB
	);
	static const std::shared_ptr<Texture> static_grey_texture = std::make_shared<Texture>(
		"textures/fallback/grey.jpg", Texture::Format::RGB
	);
	static const std::shared_ptr<Texture> static_black_texture = std::make_shared<Texture>(
		"textures/fallback/black.jpg", Texture::Format::RGB
	);
	static const std::shared_ptr<Texture> static_normal_texture = std::make_shared<Texture>(
		"textures/fallback/normal.png", Texture::Format::RGBA
	);
	assert(static_white_texture->good());
	assert(static_grey_texture->good());
	assert(static_black_texture->good());
	assert(static_normal_texture->good());
	switch (fallback_color) {
		case WHITE: return static_white_texture; break;
		case GREY: return static_grey_texture; break;
		case BLACK: return static_black_texture; break;
		case NORMAL: return static_normal_texture; break;
		default: assert(false); break;
	}
}
