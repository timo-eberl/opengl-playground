#include "scene.h"

#include <algorithm>

using namespace glpg;

const std::vector<std::shared_ptr<MeshNode>> & Scene::get_mesh_nodes() const { return m_mesh_nodes; }

void Scene::add(const std::shared_ptr<MeshNode> node) {
	m_mesh_nodes.push_back(node);
	for (auto &observer : m_observers) { observer->on_mesh_node_added(node); }
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

void Scene::add_observer(ISceneObserver * observer) const {
	m_observers.push_back(observer);
}

void Scene::remove_observer(ISceneObserver * observer) const {
	auto is_equals = [observer](ISceneObserver * o) { return observer == o; };
	// find the equal elements, move them to the end of the list and erase them
	m_observers.erase(std::remove_if(m_observers.begin(), m_observers.end(), is_equals), m_observers.end());
}
