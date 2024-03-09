#pragma once

#include <string>
#include <memory>

namespace glpg {

class Texture {
public:
	enum FallbackColor { WHITE, GREY, BLACK, NORMAL };
	enum Format {
		RED, RG, RGB, BGR, RGBA, BGRA, RED_INTEGER, RG_INTEGER, RGB_INTEGER, BGR_INTEGER,
		RGBA_INTEGER, BGRA_INTEGER, STENCIL_INDEX, DEPTH_COMPONENT, DEPTH_STENCIL
	};
	enum WrapMode { CLAMP_TO_EDGE, CLAMP_TO_BORDER, MIRRORED_REPEAT, REPEAT, MIRROR_CLAMP_TO_EDGE };
	enum MinifyingFilter {
		NEAREST, LINEAR, NEAREST_MIPMAP_NEAREST, LINEAR_MIPMAP_NEAREST, NEAREST_MIPMAP_LINEAR,
		LINEAR_MIPMAP_LINEAR
	};
	enum MagnificationFilter { MAG_NEAREST, MAG_LINEAR };

	Texture(
		const std::string& asset_path, // e.g. "textures/tex.png"
		const Format format,
		const WrapMode wrap_mode = REPEAT,
		const MinifyingFilter min_filter = LINEAR_MIPMAP_LINEAR,
		const MagnificationFilter mag_filter = MAG_LINEAR
	);
	~Texture();
	// forbid copying, because it is unclear if the texture data would be copied over
	// or read again from the file
	Texture(const Texture&) = delete;
	Texture &operator=(const Texture&) = delete;

	bool good() const; // true if texture is valid
	void reload_from_file();

	int width = 0;
	int height = 0;
	int n_channels = 0;
	unsigned char* data = nullptr;

	const std::string asset_path;
	Format format;
	WrapMode wrap_mode;
	MinifyingFilter min_filter;
	MagnificationFilter mag_filter;

	static const std::shared_ptr<Texture> get_fallback_texture(const FallbackColor fallback_color);
private:
	void load();
};

} // glpg
