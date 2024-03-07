#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h> // include glfw after glad

#include <glm/glm.hpp>

#include <unordered_map>

#include "scene.h"
#include "i_camera.h"

namespace glpg {

struct OpenGLShaderProgramGPUData { };

struct OpenGLTextureGPUData { };

struct OpenGLGeometryGPUData {
	GLuint vertex_array = 0;
	GLuint positions_buffer = 0;
	GLuint normals_buffer = 0;
	GLuint uvs_buffer = 0;
	GLuint index_buffer = 0;
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

	// ISceneObserver
	virtual void on_mesh_node_added(const std::shared_ptr<MeshNode> mesh_node) override;
	virtual void on_mesh_node_removed(const std::shared_ptr<MeshNode> mesh_node) override;
private:
	const std::weak_ptr<Scene> m_scene;

	std::unordered_map<std::shared_ptr<ShaderProgram>, OpenGLShaderProgramGPUData> m_shader_programs = {};
	std::unordered_map<std::shared_ptr<Texture>, OpenGLTextureGPUData> m_textures = {};
	std::unordered_map<std::shared_ptr<Geometry>, OpenGLGeometryGPUData> m_geometries = {};

	OpenGLGeometryGPUData setup_geometry(const Geometry &geometry) const;
	void release_geometry(OpenGLGeometryGPUData & gpu_data) const;

	bool is_last_usage(
		const std::shared_ptr<Geometry> to_be_removed_geometry,
		const std::shared_ptr<MeshNode> to_be_removed_node
	);
};

class AxesDrawer {
public:
	AxesDrawer();
	~AxesDrawer();
	// forbid copying, because it would be probably not what we want
	AxesDrawer(const AxesDrawer&) = delete;
	AxesDrawer &operator=(const AxesDrawer&) = delete;

	void draw(Scene &scene);
private:
	ShaderProgram m_shader_program = { "shaders/axes/shader.vert", "shaders/axes/shader.frag" };
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
	glpg::AxesDrawer m_axes_drawer = {};
	const ShaderProgram m_default_shader_program = { "shaders/default/shader.vert", "shaders/default/shader.frag" };
};

void opengl_set_shader_program_uniforms(const ShaderProgram &program, const Uniforms &uniforms);

} // glpg
