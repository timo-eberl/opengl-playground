#include "shader_program.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

#define ASSETS_DIR _ASSETS_DIR

using glpg::ShaderProgram;

ShaderProgram::ShaderProgram(
	const std::string& vertex_shader_path, const std::string& fragment_shader_path
)
	: m_id(0), m_good(false), m_vertex_shader_path(vertex_shader_path),
	m_fragment_shader_path(fragment_shader_path)
{
	create();
}

void ShaderProgram::recreate() { create(); }

void ShaderProgram::use() const { glUseProgram(m_id); }

bool ShaderProgram::good() const { return m_good; }

GLuint ShaderProgram::get_id() const { return m_id; }

void ShaderProgram::create() {
	m_good = true;

	const GLchar* error_vertex_shader_source =
		"#version 330 core\n"
		"layout (location = 0) in vec3 a_position;\n"
		"void main() { gl_Position = vec4(a_position.x, a_position.y, a_position.z, 1.0); }\0";
	const GLchar* error_fragment_shader_source =
		"#version 330 core\n"
		"out vec4 frag_color;\n"
		"void main() { frag_color = vec4(1.0f, 0.0f, 1.0f, 1.0f); }\0";

	const std::string v_source = read_file(ASSETS_DIR + m_vertex_shader_path);
	const std::string f_source = read_file(ASSETS_DIR + m_fragment_shader_path);

	auto vertex_shader = compile_shader(v_source.c_str(), true);
	auto fragment_shader = compile_shader(f_source.c_str(), false);

	// if compilation failed, use error shader
	if (!m_good) {
		std::cerr << "Replacing with error shader\n";
		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);
		vertex_shader = compile_shader(error_vertex_shader_source, true);
		fragment_shader = compile_shader(error_fragment_shader_source, false);
	}

	m_id = glCreateProgram();
	glAttachShader(m_id, vertex_shader);
	glAttachShader(m_id, fragment_shader);
	glLinkProgram(m_id);
	GLint success;
	glGetProgramiv(m_id, GL_LINK_STATUS, &success);
	if (!success) {
		m_good = false;

		const uint message_size = 1024;
		GLchar message[message_size];
		glGetProgramInfoLog(m_id, message_size, NULL, message);
		std::cerr << "\033[1;31m"; // font color red
		std::cerr << "Linking program failed\n" << message << std::endl;
		std::cerr << "\033[1;0m"; // reset styling
	}

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	if (m_good) {
		std::cout << "\033[1;32m"; // font color green
		std::cout << "Shader program created";
		std::cout << "\033[1;0m"; // reset styling
		std::cout << std::endl;
	}
}

std::string ShaderProgram::read_file(const std::string& path) {
	std::string v_source;

	std::ifstream v_file;
	v_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		v_file.open(path);

		std::stringstream ss;
		ss << v_file.rdbuf();
		v_source = ss.str();

		v_file.close();
	} catch (std::ifstream::failure e) {
		m_good = false;

		std::cerr << "\033[1;31m"; // font color red
		std::cerr << "Exception opening/reading/closing file \"" << path << "\": "
			<< std::strerror(errno) << "\n\n";
		std::cerr << "\033[1;0m"; // reset styling
	}

	return v_source;
}

GLuint ShaderProgram::compile_shader(const GLchar* source, const bool vertex) {
	const auto shader = glCreateShader(vertex ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);

	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		m_good = false;

		const uint message_size = 1024;
		GLchar message[message_size];
		glGetShaderInfoLog(shader, message_size, NULL, message);
		std::cerr << "\033[1;31m"; // font color red
		std::cerr << (vertex ? "Vertex" : "Fragment") << " shader compilation failed\n"
			<< message << std::endl;
		std::cerr << "\033[1;0m"; // reset styling
	}

	return shader;
}
