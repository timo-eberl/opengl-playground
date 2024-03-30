#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h> // include glfw after glad

#include <glm/glm.hpp>

#include <unordered_map>

#include "scene.h"
#include "i_camera.h"

namespace ron {

struct OpenGLGeometryGPUData {
	GLuint vertex_array = 0;
	GLuint positions_buffer = 0;
	GLuint normals_buffer = 0;
	GLuint uvs_buffer = 0;
	GLuint tangents_buffer = 0;
	GLuint index_buffer = 0;
};

struct OpenGLShaderProgramGPUData {
	GLuint id = 0;
	unsigned int last_update_count = 0;
};

struct OpenGLTextureGPUData {
	GLuint id = 0;
	unsigned int last_update_count = 0;
};

struct OpenGLDirectionalLightGPUData {
	GLuint shadow_map_framebuffer;
	GLuint shadow_map;
	unsigned int last_update_count = 0;
};

class OpenGLAxesRenderer {
public:
	OpenGLAxesRenderer();
	~OpenGLAxesRenderer();
	// forbid copying, because it would be probably not what we want
	OpenGLAxesRenderer(const OpenGLAxesRenderer&) = delete;
	OpenGLAxesRenderer &operator=(const OpenGLAxesRenderer&) = delete;

	void render();
private:
	const int m_distance = 210; // how far the axes will be rendered

	GLuint m_vertex_array = 0;
	GLuint m_positions_buffer = 0;
	GLuint m_color_buffer = 0;
};

class OpenGLGridRenderer {
public:
	OpenGLGridRenderer();
	~OpenGLGridRenderer();
	// forbid copying, because it would be probably not what we want
	OpenGLGridRenderer(const OpenGLGridRenderer&) = delete;
	OpenGLGridRenderer &operator=(const OpenGLGridRenderer&) = delete;

	void render();
private:
	// draw lines for m_distance units in one direction -> 4 * m_distance lines drawn in total
	const int m_distance = 210;

	GLuint m_vertex_array = 0;
	GLuint m_positions_buffer = 0;
	GLuint m_color_buffer = 0;
};

class OpenGLRenderer {
public:
	OpenGLRenderer(const glm::uvec2 &resolution);

	glm::uvec2 resolution;

	bool auto_clear = true;
	bool render_axes = false;
	bool render_grid = false;
	void set_clear_color(glm::vec4 clear_color);

	void preload(const Scene &scene);
	void preload(const MeshNode &mesh_node);
	void preload(const std::shared_ptr<Material> material);
	void preload(const std::shared_ptr<ShaderProgram> shader_program);
	void preload(const std::shared_ptr<Geometry> geometry);
	void preload(const std::shared_ptr<Texture> texture);
	void preload(const std::shared_ptr<const DirectionalLight> dir_light, const unsigned int update_count);

	void render(const Scene &scene, const ICamera &camera);

	void clear();
	void clear_color();
	void clear_color(glm::vec4 clear_color);
	void clear_depth();
private:
	glm::vec4 m_clear_color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	ron::OpenGLAxesRenderer m_axes_renderer = {};
	ron::OpenGLGridRenderer m_grid_renderer = {};
	// shader programs that will always be preloaded
	std::shared_ptr<ShaderProgram> m_error_shader_program = {};
	std::shared_ptr<ShaderProgram> m_axes_shader_program = {};
	std::shared_ptr<ShaderProgram> m_grid_shader_program = {};
	std::shared_ptr<ShaderProgram> m_depth_shader_program = {};
	// OpenGL specific data
	std::unordered_map<std::shared_ptr<ShaderProgram>, OpenGLShaderProgramGPUData> m_shader_programs = {};
	std::unordered_map<std::shared_ptr<Geometry>, OpenGLGeometryGPUData> m_geometries = {};
	std::unordered_map<std::shared_ptr<Texture>, OpenGLTextureGPUData> m_textures = {};
	std::unordered_map<std::shared_ptr<const DirectionalLight>, OpenGLDirectionalLightGPUData> m_directional_lights = {};

	const OpenGLShaderProgramGPUData & get_shader_program_gpu_data(
		const std::shared_ptr<ShaderProgram> shader_program
	);
	const OpenGLGeometryGPUData & get_geometry_gpu_data(const std::shared_ptr<Geometry> geometry);
	const OpenGLTextureGPUData & get_texture_gpu_data(const std::shared_ptr<Texture> texture);
	const OpenGLDirectionalLightGPUData & get_dir_light_gpu_data(
		const std::shared_ptr<const DirectionalLight> dir_light, const unsigned int update_count
	);

	void opengl_set_shader_program_uniforms(
		const OpenGLShaderProgramGPUData &program_gpu_data, const Uniforms &uniforms
	);
};

OpenGLGeometryGPUData opengl_setup_geometry(const Geometry &geometry);
void opengl_release_geometry(OpenGLGeometryGPUData &gpu_data);

OpenGLShaderProgramGPUData opengl_setup_shader_program(const ShaderProgram &shader_program);
void opengl_release_shader_program(OpenGLShaderProgramGPUData &gpu_data);

OpenGLTextureGPUData opengl_setup_texture(const Texture &texture);
void opengl_release_texture(OpenGLTextureGPUData &gpu_data);

OpenGLDirectionalLightGPUData opengl_setup_dir_light(const DirectionalLight &dir_light);
void opengl_release_dir_light(OpenGLDirectionalLightGPUData &gpu_data);

} // ron
