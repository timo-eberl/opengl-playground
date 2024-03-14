#pragma once

#include <vector>
#include <memory>
#include <cstdint>

#include <glm/glm.hpp>

#include "material.h"
#include "i_spatial.h"

namespace ron {

struct Geometry {
	std::vector<glm::vec3> positions = {};
	std::vector<glm::vec3> normals = {}; // optional - may be empty
	std::vector<glm::vec2> uvs = {}; // optional - may be empty
	std::vector<glm::vec4> tangents = {}; // optional - may be empty

	std::vector<uint32_t> indices = {};
};

std::vector<glm::vec4> generate_tangents(const Geometry &geometry);

struct MeshSection {
	std::shared_ptr<Geometry> geometry = {};
	std::shared_ptr<Material> material = {};
};

struct Mesh {
	std::vector<MeshSection> sections;
};

class MeshNode : public ISpatial {
public:
	MeshNode();
	MeshNode(const std::shared_ptr<Mesh> mesh, const glm::mat4 model_matrix);
	virtual ~MeshNode() {};

	const std::shared_ptr<Mesh> get_mesh() const;
	glm::mat3 get_normal_local_to_world_matrix() const;

	// ISpatial
	virtual glm::mat4 get_model_matrix() const override;
	virtual void set_model_matrix(glm::mat4 model_matrix) override;
private:
	std::shared_ptr<Mesh> m_mesh;
	glm::mat4 m_model_matrix;
	glm::mat3 m_normal_local_to_world_matrix;
};

} // ron
