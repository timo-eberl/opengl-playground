#include "opengl_rendering.h"

#include <glm/gtc/matrix_transform.hpp>

#include "log.h"

using namespace glpg;

void OpenGLRenderer::render(Scene &scene, const ICamera &camera, OpenGLSceneGPUData &gpu_data) {
	if (!gpu_data.belongs_to(scene)) { log::error("SceneGPUData does not match scene!"); }

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
			const auto &shader_program =
				mesh_section.material ? *mesh_section.material->shader_program : m_default_shader_program;

			glUseProgram(shader_program.get_id());

			Uniforms node_uniforms = {};
			node_uniforms.set_mat4("model_matrix", model_matrix);
			node_uniforms.set_mat3("normal_local_to_world_matrix", normal_local_to_world_matrix);

			opengl_set_shader_program_uniforms(shader_program, node_uniforms);
			opengl_set_shader_program_uniforms(shader_program, scene.global_uniforms);
			if (mesh_section.material) {
				opengl_set_shader_program_uniforms(shader_program, mesh_section.material->uniforms);
			}

			const auto &geometry_gpu_data = gpu_data.get_geometry_gpu_data(mesh_section.geometry);
			assert(geometry_gpu_data.vertex_array != 0);

			glBindVertexArray(geometry_gpu_data.vertex_array);
			glDrawElements(GL_TRIANGLES, mesh_section.geometry->indices.size(), GL_UNSIGNED_INT, NULL);

			// unbind to avoid accidental modification
			glBindVertexArray(0);
			glUseProgram(0);
		}
	}

	m_axes_drawer.draw(scene);
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
