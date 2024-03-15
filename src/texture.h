#pragma once

#include <string>

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
		const ImageData image_data, // create with image_data_from_memory or image_data_from_file
		const std::string& name,
		const MetaData meta_data = MetaData::zero_initializer(),
		const SampleData sample_data = SampleData::zero_initializer()
	);
	~Texture();
	// forbid copying
	// todo implement copying
	Texture(const Texture&) = delete;
	Texture &operator=(const Texture&) = delete;

	static ImageData image_data_from_memory(
		const unsigned char *memory, const int len
	);

	static ImageData image_data_from_file(
		const std::string &absolute_path
	);

	const std::string name;

	bool good() const; // true if texture is valid
	unsigned int get_update_count() const;

	ImageData image_data;
	MetaData meta_data;
	SampleData sample_data;

	void update(const ImageData image_data);
	void mark_as_updated(); // call this after modifying image_data
private:
	unsigned int m_update_count = 0;
};

} // ron
