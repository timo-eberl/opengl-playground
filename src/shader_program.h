#pragma once

#include <string>

namespace ron {

class ShaderProgram {
public:
	ShaderProgram(const std::string& name);
	ShaderProgram(
		const std::string& vertex_shader_source, const std::string& fragment_shader_source,
		const std::string& name
	);

	const std::string name;

	const std::string get_vertex_shader_source() const;
	const std::string get_fragment_shader_source() const;

	void update(const std::string &vertex_shader_source, const std::string &fragment_shader_source);

	unsigned int get_update_count() const;
private:
	unsigned int m_update_count = 0;

	std::string m_vertex_source = "";
	std::string m_fragment_source = "";
};

} // ron
