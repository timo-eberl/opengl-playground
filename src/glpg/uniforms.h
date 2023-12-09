#pragma once

#include <glad/glad.h>
// include glfw after glad
#include <GLFW/glfw3.h>

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
	void set_float2(const std::string name, const std::array<GLfloat, 2> v);
	void set_float3(const std::string name, const std::array<GLfloat, 3> v);
	void set_float4(const std::string name, const std::array<GLfloat, 4> v);

	void set_int1(const std::string name, const GLint v);
	void set_int2(const std::string name, const std::array<GLint, 2> v);
	void set_int3(const std::string name, const std::array<GLint, 3> v);
	void set_int4(const std::string name, const std::array<GLint, 4> v);

	void set_uint1(const std::string name, const GLuint v);
	void set_uint2(const std::string name, const std::array<GLuint, 2> v);
	void set_uint3(const std::string name, const std::array<GLuint, 3> v);
	void set_uint4(const std::string name, const std::array<GLuint, 4> v);

private:
	std::map<const std::string, GLfloat> m_float1;
	std::map<const std::string, std::array<GLfloat, 2>> m_float2;
	std::map<const std::string, std::array<GLfloat, 3>> m_float3;
	std::map<const std::string, std::array<GLfloat, 4>> m_float4;

	std::map<const std::string, GLint> m_int1;
	std::map<const std::string, std::array<GLint, 2>> m_int2;
	std::map<const std::string, std::array<GLint, 3>> m_int3;
	std::map<const std::string, std::array<GLint, 4>> m_int4;

	std::map<const std::string, GLuint> m_uint1;
	std::map<const std::string, std::array<GLuint, 2>> m_uint2;
	std::map<const std::string, std::array<GLuint, 3>> m_uint3;
	std::map<const std::string, std::array<GLuint, 4>> m_uint4;
};

} // glpg
