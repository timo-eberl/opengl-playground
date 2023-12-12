#include "uniforms.h"

using glpg::Uniforms;

void Uniforms::apply_to_program(const ShaderProgram& program) const {
	for (const auto& [key, value] : m_float1) {
		auto location = glGetUniformLocation(program.get_id(), key.c_str());
		glUniform1f(location, value[0]);
	}
	for (const auto& [key, value] : m_float2) {
		auto location = glGetUniformLocation(program.get_id(), key.c_str());
		glUniform2f(location, value[0], value[1]);
	}
	for (const auto& [key, value] : m_float3) {
		auto location = glGetUniformLocation(program.get_id(), key.c_str());
		glUniform3f(location, value[0], value[1], value[2]);
	}
	for (const auto& [key, value] : m_float4) {
		auto location = glGetUniformLocation(program.get_id(), key.c_str());
		glUniform4f(location, value[0], value[1], value[2], value[3]);
	}

	for (const auto& [key, value] : m_int1) {
		auto location = glGetUniformLocation(program.get_id(), key.c_str());
		glUniform1i(location, value[0]);
	}
	for (const auto& [key, value] : m_int2) {
		auto location = glGetUniformLocation(program.get_id(), key.c_str());
		glUniform2i(location, value[0], value[1]);
	}
	for (const auto& [key, value] : m_int3) {
		auto location = glGetUniformLocation(program.get_id(), key.c_str());
		glUniform3i(location, value[0], value[1], value[2]);
	}
	for (const auto& [key, value] : m_int4) {
		auto location = glGetUniformLocation(program.get_id(), key.c_str());
		glUniform4i(location, value[0], value[1], value[2], value[3]);
	}

	for (const auto& [key, value] : m_uint1) {
		auto location = glGetUniformLocation(program.get_id(), key.c_str());
		glUniform1ui(location, value[0]);
	}
	for (const auto& [key, value] : m_uint2) {
		auto location = glGetUniformLocation(program.get_id(), key.c_str());
		glUniform2ui(location, value[0], value[1]);
	}
	for (const auto& [key, value] : m_uint3) {
		auto location = glGetUniformLocation(program.get_id(), key.c_str());
		glUniform3ui(location, value[0], value[1], value[2]);
	}
	for (const auto& [key, value] : m_uint4) {
		auto location = glGetUniformLocation(program.get_id(), key.c_str());
		glUniform4ui(location, value[0], value[1], value[2], value[3]);
	}
}

void Uniforms::set_float1(const std::string name, const GLfloat v) {
	set_float1(name, glm::vec1(v));
}
void Uniforms::set_float1(const std::string name, const glm::vec1 v) { m_float1[name] = v; }
void Uniforms::set_float2(const std::string name, const glm::vec2 v) { m_float2[name] = v; }
void Uniforms::set_float3(const std::string name, const glm::vec3 v) { m_float3[name] = v; }
void Uniforms::set_float4(const std::string name, const glm::vec4 v) { m_float4[name] = v; }

void Uniforms::set_int1(const std::string name, const GLint v) {
	set_int1(name, glm::ivec1(v));
}
void Uniforms::set_int1(const std::string name, const glm::ivec1 v) { m_int1[name] = v; }
void Uniforms::set_int2(const std::string name, const glm::ivec2 v) { m_int2[name] = v; }
void Uniforms::set_int3(const std::string name, const glm::ivec3 v) { m_int3[name] = v; }
void Uniforms::set_int4(const std::string name, const glm::ivec4 v) { m_int4[name] = v; }

void Uniforms::set_uint1(const std::string name, const GLuint v) {
	set_uint1(name, glm::uvec1(v));
}
void Uniforms::set_uint1(const std::string name, const glm::uvec1 v) { m_uint1[name] = v; }
void Uniforms::set_uint2(const std::string name, const glm::uvec2 v) { m_uint2[name] = v; }
void Uniforms::set_uint3(const std::string name, const glm::uvec3 v) { m_uint3[name] = v; }
void Uniforms::set_uint4(const std::string name, const glm::uvec4 v) { m_uint4[name] = v; }
