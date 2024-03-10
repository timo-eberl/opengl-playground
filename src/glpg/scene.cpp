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
