#include "opengl_rendering.h"

#include <glm/gtc/type_ptr.hpp>

void glpg::opengl_set_shader_program_uniforms(const ShaderProgram &program, const Uniforms &uniforms) {
	uint texture_unit_offset = 0;
	for (const auto& [key, value] : uniforms.textures) {
		if (auto sp_tex = value.lock()) {
			if (!sp_tex->good()) {
				continue;
			}
			auto location = glGetUniformLocation(program.get_id(), key.c_str());
			glUniform1i(location, texture_unit_offset);
			glActiveTexture(GL_TEXTURE0 + texture_unit_offset);
			glBindTexture(GL_TEXTURE_2D, sp_tex->get_id());
			texture_unit_offset++;
		}
	}
	for (const auto& [key, value] : uniforms.float1) {
		auto location = glGetUniformLocation(program.get_id(), key.c_str());
		glUniform1f(location, value[0]);
	}
	for (const auto& [key, value] : uniforms.float2) {
		auto location = glGetUniformLocation(program.get_id(), key.c_str());
		glUniform2f(location, value[0], value[1]);
	}
	for (const auto& [key, value] : uniforms.float3) {
		auto location = glGetUniformLocation(program.get_id(), key.c_str());
		glUniform3f(location, value[0], value[1], value[2]);
	}
	for (const auto& [key, value] : uniforms.float4) {
		auto location = glGetUniformLocation(program.get_id(), key.c_str());
		glUniform4f(location, value[0], value[1], value[2], value[3]);
	}

	for (const auto& [key, value] : uniforms.int1) {
		auto location = glGetUniformLocation(program.get_id(), key.c_str());
		glUniform1i(location, value[0]);
	}
	for (const auto& [key, value] : uniforms.int2) {
		auto location = glGetUniformLocation(program.get_id(), key.c_str());
		glUniform2i(location, value[0], value[1]);
	}
	for (const auto& [key, value] : uniforms.int3) {
		auto location = glGetUniformLocation(program.get_id(), key.c_str());
		glUniform3i(location, value[0], value[1], value[2]);
	}
	for (const auto& [key, value] : uniforms.int4) {
		auto location = glGetUniformLocation(program.get_id(), key.c_str());
		glUniform4i(location, value[0], value[1], value[2], value[3]);
	}

	for (const auto& [key, value] : uniforms.uint1) {
		auto location = glGetUniformLocation(program.get_id(), key.c_str());
		glUniform1ui(location, value[0]);
	}
	for (const auto& [key, value] : uniforms.uint2) {
		auto location = glGetUniformLocation(program.get_id(), key.c_str());
		glUniform2ui(location, value[0], value[1]);
	}
	for (const auto& [key, value] : uniforms.uint3) {
		auto location = glGetUniformLocation(program.get_id(), key.c_str());
		glUniform3ui(location, value[0], value[1], value[2]);
	}
	for (const auto& [key, value] : uniforms.uint4) {
		auto location = glGetUniformLocation(program.get_id(), key.c_str());
		glUniform4ui(location, value[0], value[1], value[2], value[3]);
	}

	for (const auto& [key, value] : uniforms.mat2) {
		auto location = glGetUniformLocation(program.get_id(), key.c_str());
		glUniformMatrix2fv(location, 1, false, glm::value_ptr(value));
	}
	for (const auto& [key, value] : uniforms.mat3) {
		auto location = glGetUniformLocation(program.get_id(), key.c_str());
		glUniformMatrix3fv(location, 1, false, glm::value_ptr(value));
	}
	for (const auto& [key, value] : uniforms.mat4) {
		auto location = glGetUniformLocation(program.get_id(), key.c_str());
		glUniformMatrix4fv(location, 1, false, glm::value_ptr(value));
	}

	for (const auto& [key, value] : uniforms.mat2x3) {
		auto location = glGetUniformLocation(program.get_id(), key.c_str());
		glUniformMatrix2x3fv(location, 1, false, glm::value_ptr(value));
	}
	for (const auto& [key, value] : uniforms.mat3x2) {
		auto location = glGetUniformLocation(program.get_id(), key.c_str());
		glUniformMatrix3x2fv(location, 1, false, glm::value_ptr(value));
	}
	for (const auto& [key, value] : uniforms.mat2x4) {
		auto location = glGetUniformLocation(program.get_id(), key.c_str());
		glUniformMatrix2x4fv(location, 1, false, glm::value_ptr(value));
	}
	for (const auto& [key, value] : uniforms.mat4x2) {
		auto location = glGetUniformLocation(program.get_id(), key.c_str());
		glUniformMatrix4x2fv(location, 1, false, glm::value_ptr(value));
	}
	for (const auto& [key, value] : uniforms.mat3x4) {
		auto location = glGetUniformLocation(program.get_id(), key.c_str());
		glUniformMatrix3x4fv(location, 1, false, glm::value_ptr(value));
	}
	for (const auto& [key, value] : uniforms.mat4x3) {
		auto location = glGetUniformLocation(program.get_id(), key.c_str());
		glUniformMatrix4x3fv(location, 1, false, glm::value_ptr(value));
	}
}
