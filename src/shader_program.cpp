#include "shader_program.h"

#include "assets.h"

using ron::ShaderProgram;

ShaderProgram::ShaderProgram(const std::string& name) : name(name) {}

ShaderProgram::ShaderProgram(
	const std::string& vertex_shader_source, const std::string& fragment_shader_source,
	const std::string& name
)
	: m_vertex_source(vertex_shader_source), m_fragment_source(fragment_shader_source), name(name)
{}

const std::string ShaderProgram::get_vertex_shader_source() const { return m_vertex_source; }

const std::string ShaderProgram::get_fragment_shader_source() const { return m_fragment_source; }

void ShaderProgram::update(const std::string &vert_source, const std::string &frag_source) {
	if (m_vertex_source != vert_source ||m_fragment_source != frag_source) {
		m_vertex_source = vert_source;
		m_fragment_source = frag_source;
		m_update_count++;
	}
}

unsigned int ShaderProgram::get_update_count() const { return m_update_count; }
