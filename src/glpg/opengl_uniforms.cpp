#include "opengl_rendering.h"

#include <glm/gtc/type_ptr.hpp>

using namespace glpg;

void glpg::opengl_set_shader_program_uniforms(
	const OpenGLShaderProgramGPUData &program_gpu_data, const Uniforms &uniforms
) {
	for (const auto &[name, uniform] : uniforms) {
		auto location = glGetUniformLocation(program_gpu_data.id, name.c_str());
		switch (uniform->get_type()) {
			case FLOAT1: {
				const auto &value = *reinterpret_cast<const glm::vec1 *>(uniform->value_ptr());
				glUniform1f(location, value[0]);
			} break;
			case FLOAT2: {
				const auto &value = *reinterpret_cast<const glm::vec2 *>(uniform->value_ptr());
				glUniform2f(location, value[0], value[1]);
			} break;
			case FLOAT3: {
				const auto &value = *reinterpret_cast<const glm::vec3 *>(uniform->value_ptr());
				glUniform3f(location, value[0], value[1], value[2]);
			} break;

			case FLOAT4: {
				const auto &value = *reinterpret_cast<const glm::vec4 *>(uniform->value_ptr());
				glUniform4f(location, value[0], value[1], value[2], value[3]);
			} break;
			case INT1: {
				const auto &value = *reinterpret_cast<const glm::ivec1 *>(uniform->value_ptr());
				glUniform1i(location, value[0]);
			} break;
			case INT2: {
				const auto &value = *reinterpret_cast<const glm::ivec2 *>(uniform->value_ptr());
				glUniform2i(location, value[0], value[1]);
			} break;
			case INT3: {
				const auto &value = *reinterpret_cast<const glm::ivec3 *>(uniform->value_ptr());
				glUniform3i(location, value[0], value[1], value[2]);
			} break;
			case INT4: {
				const auto &value = *reinterpret_cast<const glm::ivec4 *>(uniform->value_ptr());
				glUniform4i(location, value[0], value[1], value[2], value[3]);
			} break;

			case UINT1: {
				const auto &value = *reinterpret_cast<const glm::uvec1 *>(uniform->value_ptr());
				glUniform1ui(location, value[0]);
			} break;
			case UINT2: {
				const auto &value = *reinterpret_cast<const glm::uvec2 *>(uniform->value_ptr());
				glUniform2ui(location, value[0], value[1]);
			} break;
			case UINT3: {
				const auto &value = *reinterpret_cast<const glm::uvec3 *>(uniform->value_ptr());
				glUniform3ui(location, value[0], value[1], value[2]);
			} break;
			case UINT4: {
				const auto &value = *reinterpret_cast<const glm::uvec4 *>(uniform->value_ptr());
				glUniform4ui(location, value[0], value[1], value[2], value[3]);
			} break;

			case MAT2: {
				const auto &value = *reinterpret_cast<const glm::mat2 *>(uniform->value_ptr());
				glUniformMatrix2fv(location, 1, false, glm::value_ptr(value));
			} break;
			case MAT3: {
				const auto &value = *reinterpret_cast<const glm::mat3 *>(uniform->value_ptr());
				glUniformMatrix3fv(location, 1, false, glm::value_ptr(value));
			} break;
			case MAT4: {
				const auto &value = *reinterpret_cast<const glm::mat4 *>(uniform->value_ptr());
				glUniformMatrix4fv(location, 1, false, glm::value_ptr(value));
			} break;

			case MAT2X3: {
				const auto &value = *reinterpret_cast<const glm::mat2x3 *>(uniform->value_ptr());
				glUniformMatrix2x3fv(location, 1, false, glm::value_ptr(value));
			} break;
			case MAT3X2: {
				const auto &value = *reinterpret_cast<const glm::mat3x2 *>(uniform->value_ptr());
				glUniformMatrix3x2fv(location, 1, false, glm::value_ptr(value));
			} break;
			case MAT2X4: {
				const auto &value = *reinterpret_cast<const glm::mat2x4 *>(uniform->value_ptr());
				glUniformMatrix2x4fv(location, 1, false, glm::value_ptr(value));
			} break;
			case MAT4X2: {
				const auto &value = *reinterpret_cast<const glm::mat4x2 *>(uniform->value_ptr());
				glUniformMatrix4x2fv(location, 1, false, glm::value_ptr(value));
			} break;
			case MAT3X4: {
				const auto &value = *reinterpret_cast<const glm::mat3x4 *>(uniform->value_ptr());
				glUniformMatrix3x4fv(location, 1, false, glm::value_ptr(value));
			} break;
			case MAT4X3: {
				const auto &value = *reinterpret_cast<const glm::mat4x3 *>(uniform->value_ptr());
				glUniformMatrix4x3fv(location, 1, false, glm::value_ptr(value));
			} break;

			default: assert(false); break;
		}
	}
}
