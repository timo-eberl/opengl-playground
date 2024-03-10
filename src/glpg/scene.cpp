#include "scene.h"

#include <algorithm>

using namespace glpg;

Scene::Scene() : default_material(std::make_shared<Material>()) {
	default_material->shader_program = std::make_shared<glpg::ShaderProgram>(
		"shaders/default/blinn_phong.vert", "shaders/default/blinn_phong.frag"
	);
	default_material->uniforms["albedo_tex"] = glpg::make_uniform(
		std::make_shared<Texture>("textures/fallback/white.jpg", Texture::Format::RGB)
	);
}

Scene::Scene(std::shared_ptr<Material> default_mat) : default_material(default_mat) {}

const std::vector<std::shared_ptr<MeshNode>> & Scene::get_mesh_nodes() const { return m_mesh_nodes; }

void Scene::add(const std::shared_ptr<MeshNode> node) {
	m_mesh_nodes.push_back(node);
}

void Scene::add(std::vector<std::shared_ptr<MeshNode>> nodes) {
	for (const auto & node : nodes) { add(node); }
}

void Scene::add(const Scene &scene) { add(scene.m_mesh_nodes); }

void Scene::remove(const std::shared_ptr<MeshNode> node) {
	auto is_equals = [node](std::shared_ptr<MeshNode> n) { return node == n; };
	// find the node, move it to the end of the list and erase it
	m_mesh_nodes.erase(std::remove_if(m_mesh_nodes.begin(), m_mesh_nodes.end(), is_equals), m_mesh_nodes.end());
}

void Scene::reload_all_shaders() {
	default_material->shader_program->reload_from_file();

	for (const auto &mesh_node : m_mesh_nodes) {
		for (const auto &mesh_section : mesh_node->get_mesh()->sections) {
			if (mesh_section.material && mesh_section.material->shader_program) {
				mesh_section.material->shader_program->reload_from_file();
			}
		}
	}
}

void Scene::reload_all_textures() {
	for (const auto &mesh_node : m_mesh_nodes) {
		for (const auto &mesh_section : mesh_node->get_mesh()->sections) {
			if (mesh_section.material) {
				for (const auto &[name, uniform] : mesh_section.material->uniforms) {
					if (uniform->get_type() == glpg::TEXTURE) {
						const auto &texture = *reinterpret_cast<const std::shared_ptr<Texture> *>(
							uniform->value_ptr()
						);
						texture->reload_from_file();
					}
				}
			}
		}
	}
}
