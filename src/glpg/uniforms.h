#pragma once

#include <glad/glad.h>
// include glfw after glad
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "shader_program.h"

#include <map>
#include <array>
#include <string>

namespace glpg {

class Uniforms {
public:
	// ShaderProgram::use() must be called first!
	void apply_to_program(const ShaderProgram& program) const;

	void set_float1(const std::string name, const GLfloat v);
	void set_float1(const std::string name, const glm::vec1 v);
	void set_float2(const std::string name, const glm::vec2);
	void set_float3(const std::string name, const glm::vec3);
	void set_float4(const std::string name, const glm::vec4);

	void set_int1(const std::string name, const GLint v);
	void set_int1(const std::string name, const glm::ivec1 v);
	void set_int2(const std::string name, const glm::ivec2 v);
	void set_int3(const std::string name, const glm::ivec3 v);
	void set_int4(const std::string name, const glm::ivec4 v);

	void set_uint1(const std::string name, const GLuint v);
	void set_uint1(const std::string name, const glm::uvec1 v);
	void set_uint2(const std::string name, const glm::uvec2 v);
	void set_uint3(const std::string name, const glm::uvec3 v);
	void set_uint4(const std::string name, const glm::uvec4 v);

private:
	std::map<const std::string, glm::vec1> m_float1;
	std::map<const std::string, glm::vec2> m_float2;
	std::map<const std::string, glm::vec3> m_float3;
	std::map<const std::string, glm::vec4> m_float4;

	std::map<const std::string, glm::ivec1> m_int1;
	std::map<const std::string, glm::ivec2> m_int2;
	std::map<const std::string, glm::ivec3> m_int3;
	std::map<const std::string, glm::ivec4> m_int4;

	std::map<const std::string, glm::uvec1> m_uint1;
	std::map<const std::string, glm::uvec2> m_uint2;
	std::map<const std::string, glm::uvec3> m_uint3;
	std::map<const std::string, glm::uvec4> m_uint4;
};

} // glpg
