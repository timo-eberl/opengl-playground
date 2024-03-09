#pragma once

#include <memory>
#include <vector>

#include "meshes.h"
#include "uniforms.h"

#include "i_spatial.h"

namespace glpg {

class Scene {
public:
	Uniforms global_uniforms = {};
	bool depth_test = true;

	// give read only acces to m_mesh_nodes
	const std::vector<std::shared_ptr<MeshNode>> & get_mesh_nodes() const;

	void add(const std::shared_ptr<MeshNode> node);
	void add(std::vector<std::shared_ptr<MeshNode>> nodes);
	void add(const Scene &scene);

	void remove(const std::shared_ptr<MeshNode> node);

	std::shared_ptr<ShaderProgram> default_shader_program = std::make_shared<ShaderProgram>(
		"shaders/default/blinn_phong.vert", "shaders/default/blinn_phong.frag"
	);
private:
	std::vector<std::shared_ptr<MeshNode>> m_mesh_nodes = {};
};

} // glpg
