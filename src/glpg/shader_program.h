#pragma once

#include <glad/glad.h>
// include glfw after glad
#include <GLFW/glfw3.h>

#include <string>

namespace glpg {

class ShaderProgram {
public:
	ShaderProgram(
		const std::string& vertex_shader_path, const std::string& fragment_shader_path
	);
	void recreate(); // reloads from disk, recompiles and relinks
	bool good() const; // true if creation was successful
	void use() const; // calls glUseProgram
	void set_uniform_float(const GLchar* name, const GLfloat value);

private:
	GLuint m_id;
	bool m_good;
	const std::string m_vertex_shader_path;
	const std::string m_fragment_shader_path;

	void create();
	std::string read_file(const std::string& path);
	GLuint compile_shader(const GLchar* source, const bool vertex);
};

} // glpg
