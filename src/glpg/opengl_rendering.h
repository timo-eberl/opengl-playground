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
	bool creation_successful = false;
};

// Holds OpenGL specific data for every Geometry, ShaderProgram and Texture of an associated scene.
// Data is uploaded to the GPU on constructor call and when adding MeshNodes to the associated scene.
class OpenGLSceneGPUData : public ISceneObserver {
public:
	OpenGLSceneGPUData(const std::weak_ptr<Scene> scene);
	virtual ~OpenGLSceneGPUData();
	// forbid copying, because it would be probably not what we want
	OpenGLSceneGPUData(const OpenGLSceneGPUData&) = delete;
	OpenGLSceneGPUData &operator=(const OpenGLSceneGPUData&) = delete;

	bool belongs_to(const Scene &scene) const;

	const OpenGLGeometryGPUData & get_geometry_gpu_data(const std::shared_ptr<Geometry> geometry);
	const OpenGLShaderProgramGPUData & get_shader_program_gpu_data(const std::shared_ptr<ShaderProgram> shader_program);

	// ISceneObserver
	virtual void on_mesh_node_added(const std::shared_ptr<MeshNode> mesh_node) override;
	virtual void on_mesh_node_removed(const std::shared_ptr<MeshNode> mesh_node) override;
private:
	const std::weak_ptr<Scene> m_scene;

	std::unordered_map<std::shared_ptr<Geometry>, OpenGLGeometryGPUData> m_geometries = {};
	std::unordered_map<std::shared_ptr<ShaderProgram>, OpenGLShaderProgramGPUData> m_shader_programs = {};

	OpenGLGeometryGPUData setup_geometry(const Geometry &geometry) const;
	void release_geometry(OpenGLGeometryGPUData & gpu_data) const;
	bool is_last_usage(
		const std::shared_ptr<Geometry> to_be_removed_geometry,
		const std::shared_ptr<MeshNode> to_be_removed_node
	);

	enum ShaderType { VERTEX, FRAGMENT };
	OpenGLShaderProgramGPUData setup_shader_program(const ShaderProgram &shader_program) const;
	GLuint compile_shader(
		const std::string & source, ShaderType shader_type,
		GLint *was_successful, GLchar *message, const uint message_size
	) const;
	void release_shader_program(OpenGLShaderProgramGPUData & gpu_data) const;
	bool is_last_usage(
		const std::shared_ptr<ShaderProgram> to_be_removed_shader_program,
		const std::shared_ptr<MeshNode> to_be_removed_node
	);
};

class OpenGLAxesRenderer {
public:
	OpenGLAxesRenderer();
	~OpenGLAxesRenderer();
	// forbid copying, because it would be probably not what we want
	OpenGLAxesRenderer(const OpenGLAxesRenderer&) = delete;
	OpenGLAxesRenderer &operator=(const OpenGLAxesRenderer&) = delete;

	void render(const OpenGLShaderProgramGPUData &shader_program_gpu_data, const Uniforms &uniforms);
private:
	GLuint m_vertex_array = 0;
	GLuint m_indices_buffer = 0;
	GLuint m_positions_buffer = 0;
	GLuint m_color_buffer = 0;
};

class OpenGLRenderer {
public:
	bool auto_clear = true;
	void set_clear_color(glm::vec4 clear_color);

	void render(Scene &scene, const ICamera &camera, OpenGLSceneGPUData &gpu_data);

	void clear();
	void clear_color();
	void clear_color(glm::vec4 clear_color);
	void clear_depth();
private:
	glm::vec4 m_clear_color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glpg::OpenGLAxesRenderer m_axes_drawer = {};
};

void opengl_set_shader_program_uniforms(
	const OpenGLShaderProgramGPUData &program_gpu_data, const Uniforms &uniforms
);

} // glpg
