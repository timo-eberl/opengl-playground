#include "uniforms.h"

using namespace ron;

std::shared_ptr<IUniform> ron::make_uniform(const std::shared_ptr<Texture> &value) {
	return std::make_shared<TextureUniform>(value);
}

std::shared_ptr<IUniform> ron::make_uniform(const glm::vec1 &value) {
	return std::make_shared<Float1Uniform>(value);
}
std::shared_ptr<IUniform> ron::make_uniform(const glm::vec2 &value) {
	return std::make_shared<Float2Uniform>(value);
}
std::shared_ptr<IUniform> ron::make_uniform(const glm::vec3 &value) {
	return std::make_shared<Float3Uniform>(value);
}
std::shared_ptr<IUniform> ron::make_uniform(const glm::vec4 &value) {
	return std::make_shared<Float4Uniform>(value);
}

std::shared_ptr<IUniform> ron::make_uniform(const glm::ivec1 &value) {
	return std::make_shared<Int1Uniform>(value);
}
std::shared_ptr<IUniform> ron::make_uniform(const glm::ivec2 &value) {
	return std::make_shared<Int2Uniform>(value);
}
std::shared_ptr<IUniform> ron::make_uniform(const glm::ivec3 &value) {
	return std::make_shared<Int3Uniform>(value);
}
std::shared_ptr<IUniform> ron::make_uniform(const glm::ivec4 &value) {
	return std::make_shared<Int4Uniform>(value);
}

std::shared_ptr<IUniform> ron::make_uniform(const glm::uvec1 &value) {
	return std::make_shared<UInt1Uniform>(value);
}
std::shared_ptr<IUniform> ron::make_uniform(const glm::uvec2 &value) {
	return std::make_shared<UInt2Uniform>(value);
}
std::shared_ptr<IUniform> ron::make_uniform(const glm::uvec3 &value) {
	return std::make_shared<UInt3Uniform>(value);
}
std::shared_ptr<IUniform> ron::make_uniform(const glm::uvec4 &value) {
	return std::make_shared<UInt4Uniform>(value);
}

std::shared_ptr<IUniform> ron::make_uniform(const glm::mat2 &value) {
	return std::make_shared<Mat2Uniform>(value);
}
std::shared_ptr<IUniform> ron::make_uniform(const glm::mat3 &value) {
	return std::make_shared<Mat3Uniform>(value);
}
std::shared_ptr<IUniform> ron::make_uniform(const glm::mat4 &value) {
	return std::make_shared<Mat4Uniform>(value);
}

std::shared_ptr<IUniform> ron::make_uniform(const glm::mat2x3 &value) {
	return std::make_shared<Mat2x3Uniform>(value);
}
std::shared_ptr<IUniform> ron::make_uniform(const glm::mat3x2 &value) {
	return std::make_shared<Mat3x2Uniform>(value);
}
std::shared_ptr<IUniform> ron::make_uniform(const glm::mat2x4 &value) {
	return std::make_shared<Mat2x4Uniform>(value);
}
std::shared_ptr<IUniform> ron::make_uniform(const glm::mat4x2 &value) {
	return std::make_shared<Mat4x2Uniform>(value);
}
std::shared_ptr<IUniform> ron::make_uniform(const glm::mat3x4 &value) {
	return std::make_shared<Mat3x4Uniform>(value);
}
std::shared_ptr<IUniform> ron::make_uniform(const glm::mat4x3 &value) {
	return std::make_shared<Mat4x3Uniform>(value);
}
