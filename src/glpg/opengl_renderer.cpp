#include "opengl_rendering.h"

#include <glm/gtc/matrix_transform.hpp>

#include "log.h"

using namespace glpg;

OpenGLRenderer::OpenGLRenderer() {
	static const std::shared_ptr<ShaderProgram> static_error_shader_program = std::make_shared<ShaderProgram>(
		"shaders/default/error.vert", "shaders/default/error.frag"
	);
	m_error_shader_program = static_error_shader_program;
	if (!m_shader_programs.contains(m_error_shader_program)) {
		auto error_shader_program_gpu_data = opengl_setup_shader_program(*m_error_shader_program);
		assert(error_shader_program_gpu_data.creation_successful);
		m_shader_programs.emplace(m_error_shader_program, error_shader_program_gpu_data);
	}

	static const std::shared_ptr<ShaderProgram> static_axes_shader_program = std::make_shared<ShaderProgram>(
		"shaders/default/axes.vert", "shaders/default/axes.frag"
	);
	m_axes_shader_program = static_axes_shader_program;
	if (!m_shader_programs.contains(static_axes_shader_program)) {
		auto axes_shader_program_gpu_data = opengl_setup_shader_program(*static_axes_shader_program);
		assert(axes_shader_program_gpu_data.creation_successful);
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

	scene.global_uniforms.set_mat4("view_matrix", view_matrix);
	scene.global_uniforms.set_mat4("projection_matrix", projection_matrix);
	scene.global_uniforms.set_mat4("view_projection_matrix", view_projection_matrix);
	scene.global_uniforms.set_float3("camera_world_position", camera_world_position);
	scene.global_uniforms.set_float3("directional_light_world_direction", directional_light_world_direction);

	for (const auto & mesh_node : scene.get_mesh_nodes()) {
		const auto model_matrix = mesh_node->get_model_matrix();
		const auto normal_local_to_world_matrix = mesh_node->get_normal_local_to_world_matrix();

		for (const auto & mesh_section : mesh_node->get_mesh()->sections) {
			const auto &material = mesh_section.material ?
				mesh_section.material : scene.default_material;

			const auto &shader_program = material->shader_program ?
				material->shader_program : m_error_shader_program;

			const auto &shader_program_gpu_data = get_shader_program_gpu_data(shader_program);
			glUseProgram(shader_program_gpu_data.id);

			Uniforms node_uniforms = {};
			node_uniforms.set_mat4("model_matrix", model_matrix);
			node_uniforms.set_mat3("normal_local_to_world_matrix", normal_local_to_world_matrix);

			opengl_set_shader_program_uniforms(shader_program_gpu_data, node_uniforms);
			opengl_set_shader_program_uniforms(shader_program_gpu_data, scene.global_uniforms);
			if (material) {
				opengl_set_shader_program_uniforms(shader_program_gpu_data, material->uniforms);
				set_shader_program_textures(shader_program_gpu_data, material->textures);
			}

			const auto &geometry_gpu_data = get_geometry_gpu_data(mesh_section.geometry);
			assert(geometry_gpu_data.vertex_array != 0);

			glBindVertexArray(geometry_gpu_data.vertex_array);
			glDrawElements(GL_TRIANGLES, mesh_section.geometry->indices.size(), GL_UNSIGNED_INT, NULL);

			// unbind to avoid accidental modification
			glBindVertexArray(0);
			glBindTexture(GL_TEXTURE_2D, 0);
			glUseProgram(0);
		}
	}

	if (render_axes) {
		m_axes_renderer.render(
			get_shader_program_gpu_data(m_axes_shader_program), scene.global_uniforms
		);
	}
}

void OpenGLRenderer::set_clear_color(glm::vec4 clear_color) { m_clear_color = clear_color; }

void OpenGLRenderer::clear() {
	glClearColor(m_clear_color.r, m_clear_color.g, m_clear_color.b, m_clear_color.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRenderer::clear_color() { clear_color(m_clear_color); }

void OpenGLRenderer::clear_color(glm::vec4 clear_color) {
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
	for (const auto &[name, texture] : material->textures) {
		preload(texture);
	}
}

void OpenGLRenderer::preload(const std::shared_ptr<ShaderProgram> shader_program) {
	// create gpu data if it does not exist yet
	if (!m_shader_programs.contains(shader_program)) {
		auto gpu_data = opengl_setup_shader_program(*shader_program);
		if (!gpu_data.creation_successful) {
			log::error("Shader creation failed. Replacing with error shader.", false);

			gpu_data = m_shader_programs[m_error_shader_program];
		}
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
			std::string("GPU Data of Texture ") + texture->asset_path + " not found."
			+ " Consider preloading before rendering."
		);
		preload(texture);
	}
	return m_textures[texture];
}

void OpenGLRenderer::set_shader_program_textures(
	const OpenGLShaderProgramGPUData &program_gpu_data,
	const std::map<std::string, std::shared_ptr<Texture>> &textures
) {
	uint texture_unit_offset = 0;
	for (const auto &[name, texture] : textures) {
		auto &gpu_data = get_texture_gpu_data(texture);
		if (gpu_data.id == 0) {
			continue;
		}
		auto location = glGetUniformLocation(program_gpu_data.id, name.c_str());
		glUniform1i(location, texture_unit_offset);
		glActiveTexture(GL_TEXTURE0 + texture_unit_offset);
		glBindTexture(GL_TEXTURE_2D, gpu_data.id);
		texture_unit_offset++;
	}
}
