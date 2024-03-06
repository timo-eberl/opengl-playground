#include "texture.h"

#include <stb_image.h>

#include "log.h"

#define ASSETS_DIR _ASSETS_DIR

using glpg::Texture;

Texture::Texture(
	const std::string& path,
	const GLint internal_format,
	const GLint wrap_mode,
	const GLint min_filter,
	const GLint max_filter
)
	: m_path(path), m_id(0),
	m_internal_format(internal_format),
	m_wrap_mode(wrap_mode), m_min_filter(min_filter), m_max_filter(max_filter)
{
	s_texture_units_used++;
	load();
}

Texture::~Texture() { release(); }

Texture::Texture(Texture &&other)
	: m_path(other.m_path), m_id(other.m_id), m_internal_format(other.m_internal_format),
	m_wrap_mode(other.m_wrap_mode), m_min_filter(other.m_min_filter), m_max_filter(other.m_max_filter)
{}

Texture& Texture::operator=(Texture &&other) {
	if (this == &other) return *this;

	release();
	m_path = other.m_path; m_id = other.m_id; m_internal_format = other.m_internal_format;
	m_wrap_mode = other.m_wrap_mode; m_min_filter = other.m_min_filter; m_max_filter = other.m_max_filter;

	return *this;
}

bool Texture::good() const { return m_id != 0; }

void Texture::reload() { release(); load(); }

void Texture::release() {
	if (m_id) {
		glDeleteTextures(1, &m_id);
		m_id = 0;
	}
}

GLuint Texture::get_id() const { return m_id; }

void Texture::load() {
	stbi_set_flip_vertically_on_load(true);
	int tex_width, tex_height, tex_n_channels;
	std::string path = ASSETS_DIR + m_path;
	unsigned char* texture_data = stbi_load(path.c_str(), &tex_width, &tex_height, &tex_n_channels, 0);
	if (!texture_data) {
		log::error("Failed to load texture \"" + path + "\"");
		m_id = 0;
	}
	else {
		glGenTextures(1, &m_id);
		// the texture unit here does not need to match the texture unit when binding the texture later
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_wrap_mode); // wrap mode around x
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_wrap_mode); // wrap mode around y
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_min_filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_max_filter);
		glTexImage2D(
			GL_TEXTURE_2D, 0, m_internal_format, tex_width, tex_height, 0, m_internal_format, GL_UNSIGNED_BYTE,
			texture_data
		);
		glGenerateMipmap(GL_TEXTURE_2D);

		// unbind buffer to avoid accidental modification
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	stbi_image_free(texture_data);
}
