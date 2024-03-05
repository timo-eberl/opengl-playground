#pragma once

#include <glad/glad.h>
// include glfw after glad
#include <GLFW/glfw3.h>

#include <vector>
#include <memory>
#include <cstdint>

#include <glm/glm.hpp>

#include "material.h"
#include "i_drawable.h"
#include "i_spatial.h"

namespace glpg::meshes {

struct GeometryData {
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;

	std::vector<uint32_t> indices;
};

class RenderGeometry {
public:
	RenderGeometry(const GeometryData geometry_data);
	~RenderGeometry();
	// forbid copying, because it would be probably not what we want
	RenderGeometry(const RenderGeometry&) = delete;
	RenderGeometry &operator=(const RenderGeometry&) = delete;

	void send_to_gpu();
	void draw() const;
private:
	GeometryData m_geometry_data;
	GLuint m_vertex_array = 0;
	GLuint m_positions_buffer = 0;
	GLuint m_normals_buffer = 0;
	GLuint m_uvs_buffer = 0;
	GLuint m_index_buffer = 0;

	void release();
};

struct MeshSection {
	std::shared_ptr<RenderGeometry> geometry = {};
	std::shared_ptr<Material> material = {};
};

struct Mesh {
	std::vector<MeshSection> sections;
};

class MeshNode : IDrawable, ISpatial {
public:
	MeshNode();
	MeshNode(const std::shared_ptr<Mesh> mesh);
	MeshNode(const glm::mat4 model_matrix);
	MeshNode(const std::shared_ptr<Mesh> mesh, const glm::mat4 model_matrix);
	virtual ~MeshNode() {};

	std::shared_ptr<Mesh> get_mesh() const;

	// ISpatial
	virtual glm::mat4 get_model_matrix() const override;
	virtual void set_model_matrix(glm::mat4 model_matrix) override;

	// IDrawable
	virtual void setup_drawing() override;
	virtual void draw(Uniforms &uniforms) const override;
private:
	std::shared_ptr<Mesh> m_mesh;
	glm::mat4 m_model_matrix;
	glm::mat3 m_normal_local_to_world_matrix;
};

} // glpg::meshes
