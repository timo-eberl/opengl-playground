#include "shader_program.h"

#include "assets.h"

using glpg::ShaderProgram;

ShaderProgram::ShaderProgram(
	const std::string& vertex_shader_path, const std::string& fragment_shader_path
)
	: vertex_shader_path(vertex_shader_path),
	fragment_shader_path(fragment_shader_path),
	vertex_source(assets::read_text_file(vertex_shader_path)),
	fragment_source(assets::read_text_file(fragment_shader_path))
{}
