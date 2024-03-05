#pragma once

#include <memory>
#include <vector>

#include "meshes.h"
#include "uniforms.h"

#include "i_spatial.h"

namespace glpg {

class Scene {
public:
	std::vector<std::shared_ptr<meshes::MeshNode>> mesh_nodes = {};
	Uniforms global_uniforms = {};
	bool depth_test = true;

	void add(const std::shared_ptr<meshes::MeshNode> node);
	void add(std::vector<std::shared_ptr<meshes::MeshNode>> nodes);
	void add(const Scene &scene);

	void remove(const std::shared_ptr<meshes::MeshNode> node);
};

} // glpg
