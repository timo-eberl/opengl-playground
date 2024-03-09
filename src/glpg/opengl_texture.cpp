#include "opengl_rendering.h"

#include "log.h"

using namespace glpg;

OpenGLTextureGPUData glpg::opengl_setup_texture(const Texture &texture) {
	if (!texture.good()) {
		return {};
	}
	OpenGLTextureGPUData gpu_data;

	// convert custom enums to OpenGL data
	GLenum format; switch (texture.format) {
		case Texture::RED: format = GL_RED; break;
		case Texture::RG: format = GL_RG; break;
		case Texture::RGB: format = GL_RGB; break;
		case Texture::BGR: format = GL_BGR; break;
		case Texture::RGBA: format = GL_RGBA; break;
		case Texture::BGRA: format = GL_BGRA; break;
		case Texture::RED_INTEGER: format = GL_RED_INTEGER; break;
		case Texture::RG_INTEGER: format = GL_RG_INTEGER; break;
		case Texture::RGB_INTEGER: format = GL_RGB_INTEGER; break;
		case Texture::BGR_INTEGER: format = GL_BGR_INTEGER; break;
		case Texture::RGBA_INTEGER: format = GL_RGBA_INTEGER; break;
		case Texture::BGRA_INTEGER: format = GL_BGRA_INTEGER; break;
		case Texture::STENCIL_INDEX: format = GL_STENCIL_INDEX; break;
		case Texture::DEPTH_COMPONENT: format = GL_DEPTH_COMPONENT; break;
		case Texture::DEPTH_STENCIL: format = GL_DEPTH_STENCIL; break;
		default: assert(false); break;
	}
	GLint wrap_mode; switch (texture.wrap_mode) {
		case Texture::CLAMP_TO_EDGE: wrap_mode = GL_CLAMP_TO_EDGE; break;
		case Texture::CLAMP_TO_BORDER: wrap_mode = GL_CLAMP_TO_BORDER; break;
		case Texture::MIRRORED_REPEAT: wrap_mode = GL_MIRRORED_REPEAT; break;
		case Texture::REPEAT: wrap_mode = GL_REPEAT; break;
		case Texture::MIRROR_CLAMP_TO_EDGE: wrap_mode = GL_MIRROR_CLAMP_TO_EDGE; break;
		default: assert(false); break;
	}
	GLint min_filter; switch (texture.min_filter) {
		case Texture::NEAREST: min_filter = GL_NEAREST; break;
		case Texture::LINEAR: min_filter = GL_LINEAR; break;
		case Texture::NEAREST_MIPMAP_NEAREST: min_filter = GL_NEAREST_MIPMAP_NEAREST; break;
		case Texture::LINEAR_MIPMAP_NEAREST: min_filter = GL_LINEAR_MIPMAP_NEAREST; break;
		case Texture::NEAREST_MIPMAP_LINEAR: min_filter = GL_NEAREST_MIPMAP_LINEAR; break;
		case Texture::LINEAR_MIPMAP_LINEAR: min_filter = GL_LINEAR_MIPMAP_LINEAR; break;
		default: assert(false); break;
	};
	GLint mag_filter; switch (texture.mag_filter) {
		case Texture::MAG_NEAREST: mag_filter = GL_NEAREST; break;
		case Texture::MAG_LINEAR: mag_filter = GL_LINEAR; break;
		default: assert(false); break;
	};

	glGenTextures(1, &gpu_data.id);
	// the texture unit here does not need to match the texture unit when binding the texture later
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gpu_data.id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_mode); // wrap mode around x
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_mode); // wrap mode around y
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
	glTexImage2D(
		GL_TEXTURE_2D, 0, format, texture.width, texture.height, 0,
		format, GL_UNSIGNED_BYTE, texture.data
	);
	glGenerateMipmap(GL_TEXTURE_2D);

	// unbind buffer to avoid accidental modification
	glBindTexture(GL_TEXTURE_2D, 0);

	return gpu_data;
}

void glpg::opengl_release_texture(OpenGLTextureGPUData &gpu_data) {
	if (gpu_data.id != 0) {
		glDeleteTextures(0, &gpu_data.id);
		gpu_data.id = 0;
	}
}
