#include "shader_program.h"

#include "assets.h"

using glpg::ShaderProgram;

ShaderProgram::ShaderProgram(
	const std::string& vertex_shader_path, const std::string& fragment_shader_path
)
	: vertex_shader_path(vertex_shader_path),
	fragment_shader_path(fragment_shader_path),
	m_vertex_source(assets::read_text_file(vertex_shader_path)),
	m_fragment_source(assets::read_text_file(fragment_shader_path))
{}

const std::string ShaderProgram::get_vertex_shader_source() const { return m_vertex_source; }

const std::string ShaderProgram::get_fragment_shader_source() const { return m_fragment_source; }

unsigned int ShaderProgram::get_update_count() const { return m_update_count; }

void ShaderProgram::reload_from_file() {
	m_vertex_source = assets::read_text_file(vertex_shader_path);
	m_fragment_source = assets::read_text_file(fragment_shader_path);
	m_update_count++;
}
