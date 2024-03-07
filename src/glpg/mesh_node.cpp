#include "meshes.h"

#include <glm/gtc/matrix_transform.hpp>

using namespace glpg;

MeshNode::MeshNode()
	: m_mesh(std::make_shared<Mesh>()), m_model_matrix(glm::identity<glm::mat4>()),
	m_normal_local_to_world_matrix(glm::identity<glm::mat4>()) {}

MeshNode::MeshNode(const std::shared_ptr<Mesh> mesh, const glm::mat4 model_matrix)
	: m_mesh(mesh), m_model_matrix(model_matrix),
	m_normal_local_to_world_matrix(glm::transpose(glm::inverse(glm::mat3(model_matrix)))) {}

const std::shared_ptr<Mesh> MeshNode::get_mesh() const { return m_mesh; }

glm::mat3 MeshNode::get_normal_local_to_world_matrix() const { return m_normal_local_to_world_matrix; }

glm::mat4 MeshNode::get_model_matrix() const { return m_model_matrix; }

void MeshNode::set_model_matrix(glm::mat4 model_matrix) {
	m_model_matrix = model_matrix;
	m_normal_local_to_world_matrix = glm::transpose(glm::inverse(glm::mat3(m_model_matrix)));
}
