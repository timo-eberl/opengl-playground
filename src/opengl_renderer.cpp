#include "opengl_rendering.h"

#include <glm/gtc/matrix_transform.hpp>

#include "log.h"

using namespace ron;

OpenGLRenderer::OpenGLRenderer() {
	static const std::shared_ptr<ShaderProgram> static_error_shader_program = std::make_shared<ShaderProgram>(
		"shaders/default/error.vert", "shaders/default/error.frag"
	);
	m_error_shader_program = static_error_shader_program;
	if (!m_shader_programs.contains(m_error_shader_program)) {
		auto error_shader_program_gpu_data = opengl_setup_shader_program(*m_error_shader_program);
		assert(error_shader_program_gpu_data.id != 0);
		m_shader_programs.emplace(m_error_shader_program, error_shader_program_gpu_data);
	}

	static const std::shared_ptr<ShaderProgram> static_axes_shader_program = std::make_shared<ShaderProgram>(
		"shaders/default/axes.vert", "shaders/default/axes.frag"
	);
	m_axes_shader_program = static_axes_shader_program;
	if (!m_shader_programs.contains(static_axes_shader_program)) {
		auto axes_shader_program_gpu_data = opengl_setup_shader_program(*static_axes_shader_program);
		assert(axes_shader_program_gpu_data.id != 0);
		m_shader_programs.emplace(static_axes_shader_program, axes_shader_program_gpu_data);
	}
}

void OpenGLRenderer::render(Scene &scene, const ICamera &camera) {
	if (scene.depth_test) glEnable(GL_DEPTH_TEST);
	if (auto_clear) clear();

	const auto camera_world_position = glm::vec3(camera.get_model_matrix()[3]);
	const auto view_matrix = glm::inverse(camera.get_model_matrix());
	const auto projection_matrix = camera.get_projection_matrix();
	const auto view_projection_matrix = projection_matrix * view_matrix;
	const auto directional_light_world_direction = glm::normalize(glm::vec3(4.1f, 5.9f, -1.0f));

	scene.global_uniforms["view_matrix"] = make_uniform(view_matrix);
	scene.global_uniforms["projection_matrix"] = make_uniform(projection_matrix);
	scene.global_uniforms["view_projection_matrix"] = make_uniform(view_projection_matrix);
	scene.global_uniforms["camera_world_position"] = make_uniform(camera_world_position);
	scene.global_uniforms["directional_light_world_direction"]
		= make_uniform(directional_light_world_direction);

	for (const auto & mesh_node : scene.get_mesh_nodes()) {
		const auto model_matrix = mesh_node->get_model_matrix();
		const auto normal_local_to_world_matrix = mesh_node->get_normal_local_to_world_matrix();

		for (const auto & mesh_section : mesh_node->get_mesh()->sections) {
			const auto &material = mesh_section.material ?
				mesh_section.material : scene.default_material;

			switch (material->culling_mode) {
				case Material::CullingMode::NONE:
					glDisable(GL_CULL_FACE); break;
				case Material::CullingMode::FRONT:
					glEnable(GL_CULL_FACE); glCullFace(GL_FRONT); break;
				case Material::CullingMode::BACK:
					glEnable(GL_CULL_FACE); glCullFace(GL_BACK); break;
				default: assert(false); break;
			}

			const auto &shader_program = material->shader_program ?
				material->shader_program : m_error_shader_program;

			const auto &shader_program_gpu_data = get_shader_program_gpu_data(shader_program).id != 0
				? get_shader_program_gpu_data(shader_program)
				// if the program is invalid, use the error shader program
				: get_shader_program_gpu_data(m_error_shader_program);

			glUseProgram(shader_program_gpu_data.id);

			Uniforms node_uniforms = {};
			node_uniforms["model_matrix"] = make_uniform(model_matrix);
			node_uniforms["normal_local_to_world_matrix"]
				= make_uniform(normal_local_to_world_matrix);

			opengl_set_shader_program_uniforms(shader_program_gpu_data, scene.global_uniforms);
			if (material) {
				opengl_set_shader_program_uniforms(shader_program_gpu_data, material->uniforms);
			}
			opengl_set_shader_program_uniforms(shader_program_gpu_data, node_uniforms);

			const auto &geometry_gpu_data = get_geometry_gpu_data(mesh_section.geometry);
			assert(geometry_gpu_data.vertex_array != 0);

			glEnable(GL_FRAMEBUFFER_SRGB);
			glBindVertexArray(geometry_gpu_data.vertex_array);
			glDrawElements(GL_TRIANGLES, mesh_section.geometry->indices.size(), GL_UNSIGNED_INT, NULL);

			// unbind to avoid accidental modification
			glBindVertexArray(0);
			glBindTexture(GL_TEXTURE_2D, 0);
			glUseProgram(0);
		}
	}

	if (render_axes) {
		const OpenGLShaderProgramGPUData &shader_program_gpu_data
			= get_shader_program_gpu_data(m_axes_shader_program);

		glUseProgram(shader_program_gpu_data.id);
		opengl_set_shader_program_uniforms(shader_program_gpu_data, scene.global_uniforms);

		m_axes_renderer.render();
	}
}

void OpenGLRenderer::set_clear_color(glm::vec4 clear_color) { m_clear_color = clear_color; }

void OpenGLRenderer::clear() {
	glDisable(GL_FRAMEBUFFER_SRGB);
	glClearColor(m_clear_color.r, m_clear_color.g, m_clear_color.b, m_clear_color.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRenderer::clear_color() { clear_color(m_clear_color); }

void OpenGLRenderer::clear_color(glm::vec4 clear_color) {
	glDisable(GL_FRAMEBUFFER_SRGB); 
	glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
	glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLRenderer::clear_depth() { glClear(GL_DEPTH_BUFFER_BIT); }

void OpenGLRenderer::preload(const Scene &scene) {
	preload(scene.default_material);

	for (const auto &mesh_node : scene.get_mesh_nodes()) {
		preload(*mesh_node);
	}
}

void OpenGLRenderer::preload(const MeshNode &mesh_node) {
	for (const auto &mesh_section : mesh_node.get_mesh()->sections) {
		preload(mesh_section.geometry);
		if (mesh_section.material) {
			preload(mesh_section.material);
		}
	}
}

void OpenGLRenderer::preload(const std::shared_ptr<Material> material) {
	if (material->shader_program) {
		preload(material->shader_program);
	}
	for (const auto &[name, uniform] : material->uniforms) {
		if (uniform->get_type() == UniformType::TEXTURE) {
			const auto &texture = *reinterpret_cast<const std::shared_ptr<Texture> *>(
				uniform->value_ptr()
			);
			preload(texture);
		}
	}
}

void OpenGLRenderer::preload(const std::shared_ptr<ShaderProgram> shader_program) {
	// create gpu data if it does not exist yet
	if (!m_shader_programs.contains(shader_program)) {
		auto gpu_data = opengl_setup_shader_program(*shader_program);
		m_shader_programs.emplace(shader_program, gpu_data);
	}
}

void OpenGLRenderer::preload(const std::shared_ptr<Texture> texture) {
	// create gpu data if it does not exist yet
	if (!m_textures.contains(texture)) {
		auto gpu_data = opengl_setup_texture(*texture);
		if (gpu_data.id != 0) {
			m_textures.emplace(texture, gpu_data);
		}
	}
}

void OpenGLRenderer::preload(const std::shared_ptr<Geometry> geometry) {
	// create gpu data if it does not exist yet
	if (!m_geometries.contains(geometry)) {
		m_geometries.emplace(geometry, opengl_setup_geometry(*geometry));
	}
}

const OpenGLShaderProgramGPUData & OpenGLRenderer::get_shader_program_gpu_data(
	const std::shared_ptr<ShaderProgram> shader_program
) {
	// create shader program if it does not exist yet
	if (!m_shader_programs.contains(shader_program)) {
		log::warn(
			std::string("GPU Data of ShaderProgram ")
			+ shader_program->vertex_shader_path + ", "
			+ shader_program->fragment_shader_path + "not found."
			+ " Consider preloading before rendering."
		);
		preload(shader_program);
	}
	else {
		// check if the shader program was updated, if so, send to gpu again
		auto &gpu_data = m_shader_programs[shader_program];
		if (shader_program->get_update_count() > gpu_data.last_update_count) {
			opengl_release_shader_program(gpu_data);
			m_shader_programs.erase(shader_program);
			preload(shader_program);
		}
	}
	return m_shader_programs[shader_program];
}

const OpenGLGeometryGPUData & OpenGLRenderer::get_geometry_gpu_data(
	const std::shared_ptr<Geometry> geometry
) {
	if (!m_geometries.contains(geometry)) {
		log::warn(
			"GPU Data of Geometry not found."
			" Consider preloading before rendering."
		);
		preload(geometry);
	}
	return m_geometries[geometry];
}

const OpenGLTextureGPUData & OpenGLRenderer::get_texture_gpu_data(
	const std::shared_ptr<Texture> texture
) {
	if (!m_textures.contains(texture)) {
		log::warn(
			std::string("GPU Data of Texture ") + texture->name + " not found."
			+ " Consider preloading before rendering."
		);
		preload(texture);
	}
	else {
		// check if the texture was updated, if so, send to gpu again
		auto &gpu_data = m_textures[texture];
		if (texture->get_update_count() > gpu_data.last_update_count) {
			opengl_release_texture(gpu_data);
			m_textures.erase(texture);
			preload(texture);
		}
	}
	return m_textures[texture];
}

void OpenGLRenderer::opengl_set_shader_program_uniforms(
	const OpenGLShaderProgramGPUData &program_gpu_data, const Uniforms &uniforms
) {
	uint texture_unit_offset = 0; // texture count

	for (const auto &[name, uniform] : uniforms) {
		auto location = glGetUniformLocation(program_gpu_data.id, name.c_str());
		switch (uniform->get_type()) {
			case TEXTURE: {
				const auto &texture = *reinterpret_cast<const std::shared_ptr<Texture> *>(
					uniform->value_ptr()
				);
				auto &texture_gpu_data = get_texture_gpu_data(texture);
				if (texture_gpu_data.id == 0) {
					continue; // don't bind if the texture is invalid
				}
				glUniform1i(location, texture_unit_offset);
				glActiveTexture(GL_TEXTURE0 + texture_unit_offset);
				glBindTexture(GL_TEXTURE_2D, texture_gpu_data.id);
				texture_unit_offset++;
			} break;
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
