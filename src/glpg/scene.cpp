#include "scene.h"

#include <algorithm>

using namespace glpg;

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
