#include "opengl_rendering.h"

#include <glm/gtc/matrix_transform.hpp>

#include "assets.h"
#include "log.h"

using namespace ron;

OpenGLRenderer::OpenGLRenderer(const glm::uvec2 &resolution) : resolution(resolution) {
	m_error_shader_program = assets::load_shader_program(
		"default/shaders/error.vert", "default/shaders/error.frag"
	);
	if (!m_shader_programs.contains(m_error_shader_program)) {
		auto gpu_data = opengl_setup_shader_program(*m_error_shader_program);
		assert(gpu_data.id != 0);
		m_shader_programs.emplace(m_error_shader_program, gpu_data);
	}

	m_axes_shader_program = assets::load_shader_program(
		"default/shaders/axes.vert", "default/shaders/axes.frag"
	);
	if (!m_shader_programs.contains(m_axes_shader_program)) {
		auto gpu_data = opengl_setup_shader_program(*m_axes_shader_program);
		assert(gpu_data.id != 0);
		m_shader_programs.emplace(m_axes_shader_program, gpu_data);
	}

	m_grid_shader_program = assets::load_shader_program(
		"default/shaders/grid.vert", "default/shaders/grid.frag"
	);
	if (!m_shader_programs.contains(m_grid_shader_program)) {
		auto gpu_data = opengl_setup_shader_program(*m_grid_shader_program);
		assert(gpu_data.id != 0);
		m_shader_programs.emplace(m_grid_shader_program, gpu_data);
	}

	m_depth_shader_program = assets::load_shader_program(
		"default/shaders/depth.vert", "default/shaders/depth.frag"
	);
	if (!m_shader_programs.contains(m_depth_shader_program)) {
		auto gpu_data = opengl_setup_shader_program(*m_depth_shader_program);
		assert(gpu_data.id != 0);
		m_shader_programs.emplace(m_depth_shader_program, gpu_data);
	}

	// all writes to an srgb image will assume the input is in linear space and will convert to srgb
	// -> always have this enabled
	glEnable(GL_FRAMEBUFFER_SRGB);
}

void OpenGLRenderer::render(const Scene &scene, const ICamera &camera) {
	Uniforms render_cycle_uniforms = {};

	// render shadow map
	const auto light = scene.get_directional_light();
	const auto &light_gpu_data = get_dir_light_gpu_data(
		scene.get_directional_light(), scene.get_directional_light_update_count()
	);
	auto light_space_matrix = glm::identity<glm::mat4>();
	if (light->shadow.enabled) {
		glViewport(0, 0, light->shadow.map_size.x, light->shadow.map_size.y);
		glBindFramebuffer(GL_FRAMEBUFFER, light_gpu_data.shadow_map_framebuffer);
			glClear(GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);

			const auto light_projection_matrix = glm::ortho(
				-light->shadow.frustum_size, light->shadow.frustum_size,
				-light->shadow.frustum_size, light->shadow.frustum_size,
				light->shadow.near, light->shadow.far
			);
			const auto light_view_matrix = glm::lookAt(
				light->world_position,
				light->world_position - light->world_direction,
				glm::vec3(0.0f, 1.0f, 0.0f)
			);
			light_space_matrix = light_projection_matrix * light_view_matrix;

			render_cycle_uniforms["view_projection_matrix"] = make_uniform(light_space_matrix);

			const auto &program_gpu_data = get_shader_program_gpu_data(m_depth_shader_program);
			glUseProgram(program_gpu_data.id);
			opengl_set_shader_program_uniforms(program_gpu_data, render_cycle_uniforms);

			for (const auto & mesh_node : scene.get_mesh_nodes()) {
				const auto model_matrix = mesh_node->get_model_matrix();

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

					Uniforms node_uniforms = {};
					node_uniforms["model_matrix"] = make_uniform(model_matrix);
					opengl_set_shader_program_uniforms(program_gpu_data, node_uniforms);

					const auto &geometry_gpu_data = get_geometry_gpu_data(mesh_section.geometry);
					assert(geometry_gpu_data.vertex_array != 0);

					glDisable(GL_BLEND);
					glBindVertexArray(geometry_gpu_data.vertex_array);
					glDrawElements(
						GL_TRIANGLES, mesh_section.geometry->indices.size(), GL_UNSIGNED_INT, NULL
					);

					// unbind to avoid accidental modification
					glBindVertexArray(0);
				}
			}
			glUseProgram(0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	// debug shadow map
	if (false) {
		clear();
		glViewport(0, 0, resolution.x, resolution.y);
		static bool setup_done = false;
		static GLuint vao;
		if (!setup_done) {
			static const GLfloat positions[] = {
				-3.0f, -1.0f,
				 1.0f, -1.0f,
				 1.0f,  3.0f,
			};
			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);
			GLuint pos_buffer = 0;
			glGenBuffers(1, &pos_buffer);
			glBindBuffer(GL_ARRAY_BUFFER, pos_buffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, (GLvoid*)0);
			glEnableVertexAttribArray(0);

			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			setup_done = true;
		}
		const auto shader_program = assets::load_shader_program(
			"default/shaders/debug.vert", "default/shaders/debug.frag"
		);
		const auto &shader_program_gpu_data = get_shader_program_gpu_data(shader_program);
		glUseProgram(shader_program_gpu_data.id);
		auto location = glGetUniformLocation(shader_program_gpu_data.id, "tex");
		glUniform1i(location, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, light_gpu_data.shadow_map);
		glBindVertexArray(vao);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		glBindVertexArray(0);
		glUseProgram(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		return;
	}

	glViewport(0, 0, resolution.x, resolution.y);
	if (scene.depth_test) glEnable(GL_DEPTH_TEST);
	if (auto_clear) clear();

	const auto camera_world_position = glm::vec3(camera.get_model_matrix()[3]);
	const auto view_matrix = glm::inverse(camera.get_model_matrix());
	const auto projection_matrix = camera.get_projection_matrix();
	const auto view_projection_matrix = projection_matrix * view_matrix;

	render_cycle_uniforms["view_matrix"] = make_uniform(view_matrix);
	render_cycle_uniforms["projection_matrix"] = make_uniform(projection_matrix);
	render_cycle_uniforms["view_projection_matrix"] = make_uniform(view_projection_matrix);
	render_cycle_uniforms["camera_world_position"] = make_uniform(camera_world_position);
	if (light->shadow.enabled) {
		render_cycle_uniforms["light_space_matrix"] = make_uniform(light_space_matrix);
		render_cycle_uniforms["shadow_map"] = std::make_shared<GPUTextureUniform>(
			reinterpret_cast<const void *>(&light_gpu_data.shadow_map)
		);
	}

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

			Uniforms all_uniforms = {};
			all_uniforms.insert(render_cycle_uniforms.begin(), render_cycle_uniforms.end());
			all_uniforms.insert(scene.global_uniforms.begin(), scene.global_uniforms.end());
			if (material) {
				all_uniforms.insert(material->uniforms.begin(), material->uniforms.end());
			}
			all_uniforms.insert(node_uniforms.begin(), node_uniforms.end());

			opengl_set_shader_program_uniforms(shader_program_gpu_data, all_uniforms);

			const auto &geometry_gpu_data = get_geometry_gpu_data(mesh_section.geometry);
			assert(geometry_gpu_data.vertex_array != 0);

			glDisable(GL_BLEND);
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

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glUseProgram(shader_program_gpu_data.id);
		opengl_set_shader_program_uniforms(shader_program_gpu_data, render_cycle_uniforms);

		m_axes_renderer.render();

		glUseProgram(0);
	}

	if (render_grid) {
		const OpenGLShaderProgramGPUData &shader_program_gpu_data
			= get_shader_program_gpu_data(m_grid_shader_program);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glUseProgram(shader_program_gpu_data.id);
		opengl_set_shader_program_uniforms(shader_program_gpu_data, render_cycle_uniforms);

		m_grid_renderer.render();

		glUseProgram(0);
	}
}

void OpenGLRenderer::set_clear_color(glm::vec4 clear_color) { m_clear_color = clear_color; }

void OpenGLRenderer::clear() {
	// disable srgb conversion, because clear_color is expected to already be in srgb space
	glDisable(GL_FRAMEBUFFER_SRGB);
	glClearColor(m_clear_color.r, m_clear_color.g, m_clear_color.b, m_clear_color.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_FRAMEBUFFER_SRGB);
}

void OpenGLRenderer::clear_color() { clear_color(m_clear_color); }

void OpenGLRenderer::clear_color(glm::vec4 clear_color) {
	// disable srgb conversion, because clear_color is expected to already be in srgb space
	glDisable(GL_FRAMEBUFFER_SRGB);
	glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_FRAMEBUFFER_SRGB);
}

void OpenGLRenderer::clear_depth() { glClear(GL_DEPTH_BUFFER_BIT); }

void OpenGLRenderer::preload(const Scene &scene) {
	preload(scene.default_material);
	preload(scene.get_directional_light(), scene.get_directional_light_update_count());

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

void OpenGLRenderer::preload(
	const std::shared_ptr<const DirectionalLight> dir_light, const unsigned int update_count
) {
	if (!m_directional_lights.contains(dir_light)) {
		auto gpu_data = opengl_setup_dir_light(*dir_light);
		gpu_data.last_update_count = update_count;
		m_directional_lights.emplace(dir_light, gpu_data);
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
			+ shader_program->name + " not found."
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

const OpenGLDirectionalLightGPUData & OpenGLRenderer::get_dir_light_gpu_data(
	const std::shared_ptr<const DirectionalLight> dir_light, const unsigned int update_count
) {
	if (!m_directional_lights.contains(dir_light)) {
		log::warn(
			"GPU Data of Directional Light not found. Consider preloading before rendering."
		);
		preload(dir_light, update_count);
	}
	else {
		// check if the light was updated, if so, update gpu data
		auto &gpu_data = m_directional_lights[dir_light];
		if (update_count > gpu_data.last_update_count) {
			opengl_release_dir_light(gpu_data);
			m_directional_lights.erase(dir_light);
			preload(dir_light, update_count);
		}
	}
	return m_directional_lights[dir_light];
}

void OpenGLRenderer::opengl_set_shader_program_uniforms(
	const OpenGLShaderProgramGPUData &program_gpu_data, const Uniforms &uniforms
) {
	unsigned int texture_unit_offset = 0; // texture count

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
			case GPU_TEXTURE: {
				const auto &texture_gpu_data = *reinterpret_cast<const OpenGLTextureGPUData *>(
					uniform->value_ptr()
				);
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
