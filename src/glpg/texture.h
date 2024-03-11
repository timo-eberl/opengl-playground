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
	struct ImageData {
		int width = 0;
		int height = 0;
		int n_channels = 0;
		unsigned char* data_ptr = nullptr;
	};

	Texture(
		const std::string& asset_path, // e.g. "textures/tex.png"
		const Format format,
		const bool flip = true,
		const WrapMode wrap_mode = REPEAT,
		const MinifyingFilter min_filter = LINEAR_MIPMAP_LINEAR,
		const MagnificationFilter mag_filter = MAG_LINEAR
	);
	Texture(
		const ImageData p_image_data, // create with image_data_from_memory
		const std::string& p_name,
		const Format format,
		const WrapMode wrap_mode = REPEAT,
		const MinifyingFilter min_filter = LINEAR_MIPMAP_LINEAR,
		const MagnificationFilter mag_filter = MAG_LINEAR
	);
	// the same constructors, but the format is determined automatically
	Texture(
		const std::string& asset_path, // e.g. "textures/tex.png"
		const bool flip = true,
		const WrapMode wrap_mode = REPEAT,
		const MinifyingFilter min_filter = LINEAR_MIPMAP_LINEAR,
		const MagnificationFilter mag_filter = MAG_LINEAR
	);
	Texture(
		const ImageData p_image_data, // create with image_data_from_memory
		const std::string& p_name,
		const WrapMode wrap_mode = REPEAT,
		const MinifyingFilter min_filter = LINEAR_MIPMAP_LINEAR,
		const MagnificationFilter mag_filter = MAG_LINEAR
	);
	~Texture();
	// forbid copying, because it is unclear if the texture data would be copied over
	// or read again from the file
	Texture(const Texture&) = delete;
	Texture &operator=(const Texture&) = delete;

	static ImageData image_data_from_memory(
		const unsigned char *memory, const int len, const bool flip = true
	);

	bool good() const; // true if texture is valid
	unsigned int get_update_count() const;
	void reload_from_file();

	ImageData image_data = {};

	const bool has_file;
	const std::string asset_path;
	const std::string name;
	Format format;
	WrapMode wrap_mode;
	MinifyingFilter min_filter;
	MagnificationFilter mag_filter;
private:
	const bool m_flip = true;
	unsigned int m_update_count = 0;

	void load_from_file();
};

} // glpg
