#pragma once

#include <glad/glad.h>
// include glfw after glad
#include <GLFW/glfw3.h>

#include <string>

namespace glpg {

class Texture {
public:
	Texture(
		const std::string& path, // e.g. "textures/tex.png"
		const GLint internal_format, // e.g. "GL_RGBA"
		const GLint wrap_mode = GL_REPEAT,
		const GLint min_filter = GL_LINEAR_MIPMAP_LINEAR,
		const GLint max_filter = GL_LINEAR
	);
	void reload();
	GLuint get_id() const;

private:
	inline static uint s_texture_units_used = 0; // will go up with textures used

	const std::string m_path;
	GLuint m_id;
	const GLint m_internal_format;
	const GLint m_wrap_mode;
	const GLint m_min_filter;
	const GLint m_max_filter;

	void load();
};

} // glpg
