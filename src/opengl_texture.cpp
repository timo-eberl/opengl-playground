#include "opengl_rendering.h"

#include "log.h"

using namespace ron;

OpenGLTextureGPUData ron::opengl_setup_texture(const Texture &texture) {
	if (!texture.good()) {
		return {};
	}
	OpenGLTextureGPUData gpu_data;

	// convert custom enums to OpenGL data
	GLenum internal_format;
	GLenum format;
	auto channels = texture.meta_data.channels;
	if (texture.meta_data.channels == Texture::Channels::AUTOMATIC) {
		switch (texture.image_data.n_channels) {
			case 1: channels = Texture::Channels::R; break;
			case 2: channels = Texture::Channels::RG; break;
			case 3: channels = Texture::Channels::RGB; break;
			case 4: channels = Texture::Channels::RGBA; break;
			default: assert(false); break;
		}
	}
	switch (channels) {
		case Texture::R: internal_format = GL_RED; format = GL_RED; break;
		case Texture::RG: internal_format = GL_RG; format = GL_RG; break;
		case Texture::RGB: {
			format = GL_RGB;
			switch (texture.meta_data.color_space) {
				case Texture::ColorSpace::SRGB: internal_format = GL_SRGB; break;
				case Texture::ColorSpace::NON_COLOR: internal_format = GL_RGB; break;
				default: assert(false); break;
			}
		} break;
		case Texture::RGBA: {
			format = GL_RGBA;
			switch (texture.meta_data.color_space) {
				case Texture::ColorSpace::SRGB: internal_format = GL_SRGB_ALPHA; break;
				case Texture::ColorSpace::NON_COLOR: internal_format = GL_RGBA; break;
				default: assert(false); break;
			}
		} break;
		case Texture::BGR: internal_format = GL_BGR; format = GL_BGR; break;
		case Texture::BGRA: internal_format = GL_BGRA; format = GL_BGRA; break;
		default: assert(false); break;
	}
	GLint wrap_mode_s; switch (texture.sample_data.wrap_mode_s) {
		case Texture::CLAMP_TO_EDGE: wrap_mode_s = GL_CLAMP_TO_EDGE; break;
		case Texture::CLAMP_TO_BORDER: wrap_mode_s = GL_CLAMP_TO_BORDER; break;
		case Texture::MIRRORED_REPEAT: wrap_mode_s = GL_MIRRORED_REPEAT; break;
		case Texture::REPEAT: wrap_mode_s = GL_REPEAT; break;
		case Texture::MIRROR_CLAMP_TO_EDGE: wrap_mode_s = GL_MIRROR_CLAMP_TO_EDGE; break;
		default: assert(false); break;
	}
	GLint wrap_mode_t; switch (texture.sample_data.wrap_mode_t) {
		case Texture::CLAMP_TO_EDGE: wrap_mode_t = GL_CLAMP_TO_EDGE; break;
		case Texture::CLAMP_TO_BORDER: wrap_mode_t = GL_CLAMP_TO_BORDER; break;
		case Texture::MIRRORED_REPEAT: wrap_mode_t = GL_MIRRORED_REPEAT; break;
		case Texture::REPEAT: wrap_mode_t = GL_REPEAT; break;
		case Texture::MIRROR_CLAMP_TO_EDGE: wrap_mode_t = GL_MIRROR_CLAMP_TO_EDGE; break;
		default: assert(false); break;
	}
	GLint min_filter; switch (texture.sample_data.min_filter) {
		case Texture::NEAREST: min_filter = GL_NEAREST; break;
		case Texture::LINEAR: min_filter = GL_LINEAR; break;
		case Texture::NEAREST_MIPMAP_NEAREST: min_filter = GL_NEAREST_MIPMAP_NEAREST; break;
		case Texture::LINEAR_MIPMAP_NEAREST: min_filter = GL_LINEAR_MIPMAP_NEAREST; break;
		case Texture::NEAREST_MIPMAP_LINEAR: min_filter = GL_NEAREST_MIPMAP_LINEAR; break;
		case Texture::LINEAR_MIPMAP_LINEAR: min_filter = GL_LINEAR_MIPMAP_LINEAR; break;
		default: assert(false); break;
	};
	GLint mag_filter; switch (texture.sample_data.mag_filter) {
		case Texture::MAG_NEAREST: mag_filter = GL_NEAREST; break;
		case Texture::MAG_LINEAR: mag_filter = GL_LINEAR; break;
		default: assert(false); break;
	};
	glGenerateMipmap(GL_TEXTURE_2D);
	glGenTextures(1, &gpu_data.id);
	// the texture unit here does not need to match the texture unit when binding the texture later
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gpu_data.id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_mode_s); // wrap mode around x
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_mode_t); // wrap mode around y
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
	glTexImage2D(
		GL_TEXTURE_2D, 0, internal_format, texture.image_data.width, texture.image_data.height, 0,
		format, GL_UNSIGNED_BYTE, texture.image_data.data_ptr
	);
	glGenerateMipmap(GL_TEXTURE_2D);

	// unbind buffer to avoid accidental modification
	glBindTexture(GL_TEXTURE_2D, 0);

	gpu_data.last_update_count = texture.get_update_count();
	return gpu_data;
}

void ron::opengl_release_texture(OpenGLTextureGPUData &gpu_data) {
	if (gpu_data.id != 0) {
		glDeleteTextures(0, &gpu_data.id);
		gpu_data.id = 0;
	}
}
