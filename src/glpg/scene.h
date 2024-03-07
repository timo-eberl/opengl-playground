#pragma once

#include <memory>
#include <vector>

#include "meshes.h"
#include "uniforms.h"

#include "i_spatial.h"

namespace glpg {

class ISceneObserver {
public:
	virtual ~ISceneObserver() {}
	virtual void on_mesh_node_added(const std::shared_ptr<MeshNode> mesh_node) = 0;
	virtual void on_mesh_node_removed(const std::shared_ptr<MeshNode> mesh_node) = 0;
};

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

	void add_observer(ISceneObserver * observer) const;
	void remove_observer(ISceneObserver * observer) const;
private:
	std::vector<std::shared_ptr<MeshNode>> m_mesh_nodes = {};
	mutable std::vector<ISceneObserver *> m_observers = {};
};

} // glpg
