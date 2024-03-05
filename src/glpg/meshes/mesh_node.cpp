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

void MeshNode::setup_drawing() {
	for (auto & mesh_section : m_mesh->sections) {
		mesh_section.geometry->send_to_gpu();

		// make sure a material and shader program is set
		if (!mesh_section.material) {
			mesh_section.material = std::make_shared<Material>(Uniforms(), nullptr);
		}
		if (!mesh_section.material->shader_program) {
			mesh_section.material->shader_program = std::make_shared<ShaderProgram>(
				"shaders/default/shader.vert", "shaders/default/shader.frag"
			);
		}
	}
}

void MeshNode::draw(Uniforms &uniforms) const {
	for (auto & mesh_section : m_mesh->sections) {
		const auto shader_program = mesh_section.material->shader_program;
		glUseProgram(shader_program->get_id());

		mesh_section.material->uniforms.set_mat4("model_matrix", m_model_matrix);
		mesh_section.material->uniforms.apply_to_program(*shader_program);

		uniforms.apply_to_program(*shader_program);

		mesh_section.geometry->draw();

		glUseProgram(0);
	}
}
