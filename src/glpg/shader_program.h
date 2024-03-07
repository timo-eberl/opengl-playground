#pragma once

#include <string>

namespace glpg {

class ShaderProgram {
public:
	ShaderProgram(
		const std::string& vertex_shader_path, const std::string& fragment_shader_path
	);

	std::string vertex_source = "";
	std::string fragment_source = "";

	void reload_from_file();

	const std::string vertex_shader_path;
	const std::string fragment_shader_path;
};

} // glpg
