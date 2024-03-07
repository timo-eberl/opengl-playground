#include "uniforms.h"

using glpg::Uniforms;

void Uniforms::set_texture(const std::string& name, const std::weak_ptr<glpg::Texture> t) {
	textures[name] = t;
}
void Uniforms::set_float1(const std::string& name, const float v) {
	set_float1(name, glm::vec1(v));
}
void Uniforms::set_float1(const std::string& name, const glm::vec1 v) { float1[name] = v; }
void Uniforms::set_float2(const std::string& name, const glm::vec2 v) { float2[name] = v; }
void Uniforms::set_float3(const std::string& name, const glm::vec3 v) { float3[name] = v; }
void Uniforms::set_float4(const std::string& name, const glm::vec4 v) { float4[name] = v; }

void Uniforms::set_int1(const std::string& name, const int v) {
	set_int1(name, glm::ivec1(v));
}
void Uniforms::set_int1(const std::string& name, const glm::ivec1 v) { int1[name] = v; }
void Uniforms::set_int2(const std::string& name, const glm::ivec2 v) { int2[name] = v; }
void Uniforms::set_int3(const std::string& name, const glm::ivec3 v) { int3[name] = v; }
void Uniforms::set_int4(const std::string& name, const glm::ivec4 v) { int4[name] = v; }

void Uniforms::set_uint1(const std::string& name, const unsigned int v) {
	set_uint1(name, glm::uvec1(v));
}
void Uniforms::set_uint1(const std::string& name, const glm::uvec1 v) { uint1[name] = v; }
void Uniforms::set_uint2(const std::string& name, const glm::uvec2 v) { uint2[name] = v; }
void Uniforms::set_uint3(const std::string& name, const glm::uvec3 v) { uint3[name] = v; }
void Uniforms::set_uint4(const std::string& name, const glm::uvec4 v) { uint4[name] = v; }

void Uniforms::set_mat2(const std::string& name, const glm::mat2 v) { mat2[name] = v; }
void Uniforms::set_mat3(const std::string& name, const glm::mat3 v) { mat3[name] = v; }
void Uniforms::set_mat4(const std::string& name, const glm::mat4 v) { mat4[name] = v; }

void Uniforms::set_mat2x3(const std::string& name, const glm::mat2x3 v) { mat2x3[name] = v; }
void Uniforms::set_mat3x2(const std::string& name, const glm::mat3x2 v) { mat3x2[name] = v; }
void Uniforms::set_mat2x4(const std::string& name, const glm::mat2x4 v) { mat2x4[name] = v; }
void Uniforms::set_mat4x2(const std::string& name, const glm::mat4x2 v) { mat4x2[name] = v; }
void Uniforms::set_mat3x4(const std::string& name, const glm::mat3x4 v) { mat3x4[name] = v; }
void Uniforms::set_mat4x3(const std::string& name, const glm::mat4x3 v) { mat4x3[name] = v; }
