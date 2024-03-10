#include "uniforms.h"

using namespace glpg;

std::shared_ptr<IUniform> glpg::make_uniform(glm::vec1 value) {
	return std::make_shared<Float1Uniform>(value);
}
std::shared_ptr<IUniform> glpg::make_uniform(glm::vec2 value) {
	return std::make_shared<Float2Uniform>(value);
}
std::shared_ptr<IUniform> glpg::make_uniform(glm::vec3 value) {
	return std::make_shared<Float3Uniform>(value);
}
std::shared_ptr<IUniform> glpg::make_uniform(glm::vec4 value) {
	return std::make_shared<Float4Uniform>(value);
}

std::shared_ptr<IUniform> glpg::make_uniform(glm::ivec1 value) {
	return std::make_shared<Int1Uniform>(value);
}
std::shared_ptr<IUniform> glpg::make_uniform(glm::ivec2 value) {
	return std::make_shared<Int2Uniform>(value);
}
std::shared_ptr<IUniform> glpg::make_uniform(glm::ivec3 value) {
	return std::make_shared<Int3Uniform>(value);
}
std::shared_ptr<IUniform> glpg::make_uniform(glm::ivec4 value) {
	return std::make_shared<Int4Uniform>(value);
}

std::shared_ptr<IUniform> glpg::make_uniform(glm::uvec1 value) {
	return std::make_shared<UInt1Uniform>(value);
}
std::shared_ptr<IUniform> glpg::make_uniform(glm::uvec2 value) {
	return std::make_shared<UInt2Uniform>(value);
}
std::shared_ptr<IUniform> glpg::make_uniform(glm::uvec3 value) {
	return std::make_shared<UInt3Uniform>(value);
}
std::shared_ptr<IUniform> glpg::make_uniform(glm::uvec4 value) {
	return std::make_shared<UInt4Uniform>(value);
}

std::shared_ptr<IUniform> glpg::make_uniform(glm::mat2 value) {
	return std::make_shared<Mat2Uniform>(value);
}
std::shared_ptr<IUniform> glpg::make_uniform(glm::mat3 value) {
	return std::make_shared<Mat3Uniform>(value);
}
std::shared_ptr<IUniform> glpg::make_uniform(glm::mat4 value) {
	return std::make_shared<Mat4Uniform>(value);
}

std::shared_ptr<IUniform> glpg::make_uniform(glm::mat2x3 value) {
	return std::make_shared<Mat2x3Uniform>(value);
}
std::shared_ptr<IUniform> glpg::make_uniform(glm::mat3x2 value) {
	return std::make_shared<Mat3x2Uniform>(value);
}
std::shared_ptr<IUniform> glpg::make_uniform(glm::mat2x4 value) {
	return std::make_shared<Mat2x4Uniform>(value);
}
std::shared_ptr<IUniform> glpg::make_uniform(glm::mat4x2 value) {
	return std::make_shared<Mat4x2Uniform>(value);
}
std::shared_ptr<IUniform> glpg::make_uniform(glm::mat3x4 value) {
	return std::make_shared<Mat3x4Uniform>(value);
}
std::shared_ptr<IUniform> glpg::make_uniform(glm::mat4x3 value) {
	return std::make_shared<Mat4x3Uniform>(value);
}
