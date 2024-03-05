#include "glpg/meshes.h"

#include <glm/gtc/matrix_transform.hpp>

using namespace glpg::meshes;

MeshNode::MeshNode() : m_mesh(std::make_shared<Mesh>()), m_model_matrix(glm::identity<glm::mat4>()) {}

MeshNode::MeshNode(const std::shared_ptr<Mesh> mesh) : m_mesh(mesh), m_model_matrix(glm::identity<glm::mat4>()) {}

MeshNode::MeshNode(const glm::mat4 model_matrix) : m_mesh(std::make_shared<Mesh>()), m_model_matrix(model_matrix) {}

MeshNode::MeshNode(const std::shared_ptr<Mesh> mesh, const glm::mat4 model_matrix)
	: m_mesh(mesh), m_model_matrix(model_matrix) {}

std::shared_ptr<Mesh> MeshNode::get_mesh() const { return m_mesh; }

glm::mat4 MeshNode::get_model_matrix() const { return m_model_matrix; }

void MeshNode::set_model_matrix(glm::mat4 model_matrix) { m_model_matrix = model_matrix; }
