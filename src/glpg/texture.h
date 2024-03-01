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
	~Texture();
	// forbid copying, because it would be incredibly expensive!
	Texture(const Texture&) = delete;
	Texture &operator=(const Texture&) = delete;

	// move semantics
	Texture(Texture &&other);
	Texture& operator=(Texture &&other);

	bool good() const; // true if texture is valid
	void reload();
	GLuint get_id() const;

private:
	inline static uint s_texture_units_used = 0; // will go up with textures used

	std::string m_path;
	GLuint m_id;
	GLint m_internal_format;
	GLint m_wrap_mode;
	GLint m_min_filter;
	GLint m_max_filter;

	void load();
	void release();
};

} // glpg
