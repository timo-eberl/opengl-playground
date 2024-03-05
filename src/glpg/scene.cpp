#include "scene.h"

#include <algorithm>

using namespace glpg;

void Scene::add(const std::shared_ptr<meshes::MeshNode> node) {
	mesh_nodes.push_back(node);
	node->setup_drawing();
}

void Scene::add(std::vector<std::shared_ptr<meshes::MeshNode>> nodes) {
	mesh_nodes.insert(mesh_nodes.end(), nodes.begin(), nodes.end());
}

void Scene::add(const Scene &scene) {
	add(scene.mesh_nodes);
}

void Scene::remove(const std::shared_ptr<meshes::MeshNode> node) {
	auto is_equals = [node](std::shared_ptr<meshes::MeshNode> n) { return node == n; };
	// find the node, move it to the end of the list and erase it
	mesh_nodes.erase(std::remove_if(mesh_nodes.begin(), mesh_nodes.end(), is_equals), mesh_nodes.end());
}
