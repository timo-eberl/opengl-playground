#pragma once

#include <memory>
#include <vector>

#include "meshes.h"
#include "uniforms.h"
#include "lights.h"
#include "i_spatial.h"

namespace ron {

class Scene {
public:
	Scene();
	Scene(std::shared_ptr<Material> default_mat);

	Uniforms global_uniforms = {};
	bool depth_test = true;

	void set_directional_light(const DirectionalLight &directional_light);
	std::shared_ptr<const DirectionalLight> get_directional_light() const;
	unsigned int get_directional_light_update_count() const;

	// give read only acces to m_mesh_nodes
	const std::vector<std::shared_ptr<MeshNode>> & get_mesh_nodes() const;

	void add(const std::shared_ptr<MeshNode> node);
	void add(std::vector<std::shared_ptr<MeshNode>> nodes);
	void add(const Scene &scene);

	void remove(const std::shared_ptr<MeshNode> node);

	std::shared_ptr<Material> default_material;
private:
	std::vector<std::shared_ptr<MeshNode>> m_mesh_nodes = {};

	std::shared_ptr<DirectionalLight> m_directional_light = std::make_shared<DirectionalLight>();
	unsigned int m_directional_light_update_count = 0;
};

} // ron
