#pragma once

#include <string>

namespace glpg {

class ShaderProgram {
public:
	ShaderProgram(
		const std::string& vertex_shader_path, const std::string& fragment_shader_path
	);

	const std::string vertex_source = "";
	const std::string fragment_source = "";

	const std::string vertex_shader_path;
	const std::string fragment_shader_path;
};

} // glpg
