#pragma once

#include <glm/glm.hpp>

#include "shader_program.h"
#include "texture.h"

#include <map>
#include <array>
#include <memory>
#include <string>

namespace glpg {

// holds uniform data and has some convenience methods for setting them
struct Uniforms {
	void set_texture(const std::string& name, const std::weak_ptr<glpg::Texture> t);

	void set_float1(const std::string& name, const float v);
	void set_float1(const std::string& name, const glm::vec1 v);
	void set_float2(const std::string& name, const glm::vec2);
	void set_float3(const std::string& name, const glm::vec3);
	void set_float4(const std::string& name, const glm::vec4);

	void set_int1(const std::string& name, const int v);
	void set_int1(const std::string& name, const glm::ivec1 v);
	void set_int2(const std::string& name, const glm::ivec2 v);
	void set_int3(const std::string& name, const glm::ivec3 v);
	void set_int4(const std::string& name, const glm::ivec4 v);

	void set_uint1(const std::string& name, const unsigned int v);
	void set_uint1(const std::string& name, const glm::uvec1 v);
	void set_uint2(const std::string& name, const glm::uvec2 v);
	void set_uint3(const std::string& name, const glm::uvec3 v);
	void set_uint4(const std::string& name, const glm::uvec4 v);

	void set_mat2(const std::string& name, const glm::mat2 v);
	void set_mat3(const std::string& name, const glm::mat3 v);
	void set_mat4(const std::string& name, const glm::mat4 v);

	void set_mat2x3(const std::string& name, const glm::mat2x3 v);
	void set_mat3x2(const std::string& name, const glm::mat3x2 v);
	void set_mat2x4(const std::string& name, const glm::mat2x4 v);
	void set_mat4x2(const std::string& name, const glm::mat4x2 v);
	void set_mat3x4(const std::string& name, const glm::mat3x4 v);
	void set_mat4x3(const std::string& name, const glm::mat4x3 v);

	std::map<const std::string, std::weak_ptr<glpg::Texture>> textures;

	std::map<const std::string, glm::vec1> float1;
	std::map<const std::string, glm::vec2> float2;
	std::map<const std::string, glm::vec3> float3;
	std::map<const std::string, glm::vec4> float4;

	std::map<const std::string, glm::ivec1> int1;
	std::map<const std::string, glm::ivec2> int2;
	std::map<const std::string, glm::ivec3> int3;
	std::map<const std::string, glm::ivec4> int4;

	std::map<const std::string, glm::uvec1> uint1;
	std::map<const std::string, glm::uvec2> uint2;
	std::map<const std::string, glm::uvec3> uint3;
	std::map<const std::string, glm::uvec4> uint4;

	std::map<const std::string, glm::mat2> mat2;
	std::map<const std::string, glm::mat3> mat3;
	std::map<const std::string, glm::mat4> mat4;

	std::map<const std::string, glm::mat2x3> mat2x3;
	std::map<const std::string, glm::mat3x2> mat3x2;
	std::map<const std::string, glm::mat2x4> mat2x4;
	std::map<const std::string, glm::mat4x2> mat4x2;
	std::map<const std::string, glm::mat3x4> mat3x4;
	std::map<const std::string, glm::mat4x3> mat4x3;
};

} // glpg
