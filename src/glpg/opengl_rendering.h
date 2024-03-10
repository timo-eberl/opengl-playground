#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h> // include glfw after glad

#include <glm/glm.hpp>

#include <unordered_map>

#include "scene.h"
#include "i_camera.h"

namespace glpg {

struct OpenGLGeometryGPUData {
	GLuint vertex_array = 0;
	GLuint positions_buffer = 0;
	GLuint normals_buffer = 0;
	GLuint uvs_buffer = 0;
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

class OpenGLAxesRenderer {
public:
	OpenGLAxesRenderer();
	~OpenGLAxesRenderer();
	// forbid copying, because it would be probably not what we want
	OpenGLAxesRenderer(const OpenGLAxesRenderer&) = delete;
	OpenGLAxesRenderer &operator=(const OpenGLAxesRenderer&) = delete;

	void render();
private:
	GLuint m_vertex_array = 0;
	GLuint m_indices_buffer = 0;
	GLuint m_positions_buffer = 0;
	GLuint m_color_buffer = 0;
};

class OpenGLRenderer {
public:
	OpenGLRenderer();

	bool auto_clear = true;
	bool render_axes = true;
	void set_clear_color(glm::vec4 clear_color);

	void preload(const Scene &scene);
	void preload(const MeshNode &mesh_node);
	void preload(const std::shared_ptr<Material> material);
	void preload(const std::shared_ptr<ShaderProgram> shader_program);
	void preload(const std::shared_ptr<Geometry> geometry);
	void preload(const std::shared_ptr<Texture> texture);

	void render(Scene &scene, const ICamera &camera);

	void clear();
	void clear_color();
	void clear_color(glm::vec4 clear_color);
	void clear_depth();
private:
	glm::vec4 m_clear_color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glpg::OpenGLAxesRenderer m_axes_renderer = {};
	// shader programs that will always be preloaded
	std::shared_ptr<ShaderProgram> m_error_shader_program = {};
	std::shared_ptr<ShaderProgram> m_axes_shader_program = {};
	// OpenGL specific data
	std::unordered_map<std::shared_ptr<ShaderProgram>, OpenGLShaderProgramGPUData> m_shader_programs = {};
	std::unordered_map<std::shared_ptr<Geometry>, OpenGLGeometryGPUData> m_geometries = {};
	std::unordered_map<std::shared_ptr<Texture>, OpenGLTextureGPUData> m_textures = {};

	const OpenGLShaderProgramGPUData & get_shader_program_gpu_data(
		const std::shared_ptr<ShaderProgram> shader_program
	);
	const OpenGLGeometryGPUData & get_geometry_gpu_data(const std::shared_ptr<Geometry> geometry);
	const OpenGLTextureGPUData & get_texture_gpu_data(const std::shared_ptr<Texture> texture);

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

} // glpg
