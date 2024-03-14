#pragma once

#include <string>
#include <memory>

namespace ron {

class Texture {
public:
	enum Channels { AUTOMATIC, R, RG, RGB, RGBA, BGR, BGRA };
	enum ColorSpace { SRGB, NON_COLOR };
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
		static ImageData zero_initializer() { static ImageData zero = {}; return zero; }
	};
	struct MetaData {
		Channels channels = AUTOMATIC;
		ColorSpace color_space = SRGB;
		bool flip_y = true;
		static MetaData zero_initializer() { static MetaData zero = {}; return zero; }
	};
	struct SampleData {
		WrapMode wrap_mode_s = REPEAT;
		WrapMode wrap_mode_t = REPEAT;
		MinifyingFilter min_filter = LINEAR_MIPMAP_LINEAR;
		MagnificationFilter mag_filter = MAG_LINEAR;
		static SampleData zero_initializer() { static SampleData zero = {}; return zero; }
	};

	Texture(
		const std::string& asset_path, // e.g. "textures/tex.png"
		const MetaData meta_data = MetaData::zero_initializer(),
		const SampleData sample_data = SampleData::zero_initializer()
	);
	Texture(
		const ImageData image_data, // can be created with image_data_from_memory
		const std::string& name,
		const MetaData meta_data = MetaData::zero_initializer(),
		const SampleData sample_data = SampleData::zero_initializer()
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

	ImageData image_data;
	MetaData meta_data;
	SampleData sample_data;

	const bool has_file;
	const std::string asset_path;
	const std::string name;
private:
	unsigned int m_update_count = 0;

	void load_from_file();
};

} // ron
