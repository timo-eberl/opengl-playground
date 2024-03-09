#include "opengl_rendering.h"

#include "log.h"

using namespace glpg;

static GLuint compile_shader(
	const std::string & source, const GLenum shader_type,
	GLint *was_successful, GLchar *message, const uint message_size
) {
	GLuint shader = glCreateShader(shader_type);

	const GLchar* source_c_str = source.c_str();
	glShaderSource(shader, 1, &source_c_str, NULL);
	glCompileShader(shader);

	glGetShaderiv(shader, GL_COMPILE_STATUS, was_successful);
	if (!*was_successful) {
		glGetShaderInfoLog(shader, message_size, NULL, message);
	}

	return shader;
}

OpenGLShaderProgramGPUData glpg::opengl_setup_shader_program(const ShaderProgram &shader_program) {
	const uint message_size = 1024;
	GLchar message[message_size];

	GLint vertex_compilation_success = false;
	auto vertex_shader = compile_shader(
		shader_program.vertex_source.c_str(), GL_VERTEX_SHADER,
		&vertex_compilation_success, message, message_size
	);
	if (!vertex_compilation_success) {
		log::error(
			std::string("Vertex shader compilation failed (")
			+ shader_program.vertex_shader_path + "):" , false
		);
		log::error(message);
	}

	GLint fragment_compilation_success = false;
	auto fragment_shader = compile_shader(
		shader_program.fragment_source.c_str(), GL_FRAGMENT_SHADER,
		&fragment_compilation_success, message, message_size
	);
	if (!fragment_compilation_success) {
		log::error(
			std::string("Fragment shader compilation failed (")
			+ shader_program.fragment_shader_path + "):" , false
		);
		log::error(message);
	}

	// check if compilation failed
	if (!vertex_compilation_success || !fragment_compilation_success) {
		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);
		return {};
	}

	GLuint program_id = glCreateProgram();
	glAttachShader(program_id, vertex_shader);
	glAttachShader(program_id, fragment_shader);
	glLinkProgram(program_id);
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	GLint linkage_success;
	glGetProgramiv(program_id, GL_LINK_STATUS, &linkage_success);

	if (!linkage_success) {
		const uint message_size = 1024;
		GLchar message[message_size];
		glGetProgramInfoLog(program_id, message_size, NULL, message);

		log::error(
			std::string("Linking shader program failed (")
			+ shader_program.vertex_shader_path + ", "
			+ shader_program.fragment_shader_path + "):" , false
		);
		log::error(std::string(message));

		glDeleteProgram(program_id);
		return {};
	}

	return { program_id, true };
}

void glpg::opengl_release_shader_program(OpenGLShaderProgramGPUData & gpu_data) {
	glDeleteProgram(gpu_data.id);
	gpu_data = {};
}
