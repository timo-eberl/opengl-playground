#pragma once

#include <string>

namespace glpg {

class ShaderProgram {
public:
	ShaderProgram(
		const std::string& vertex_shader_path, const std::string& fragment_shader_path
	);

	const std::string vertex_shader_path;
	const std::string fragment_shader_path;

	const std::string get_vertex_shader_source() const;
	const std::string get_fragment_shader_source() const;

	unsigned int get_update_count() const;
	void reload_from_file();
private:
	unsigned int m_update_count = 0;

	std::string m_vertex_source = "";
	std::string m_fragment_source = "";
};

} // glpg
