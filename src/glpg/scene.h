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

	// the error shader will always be preloaded
	const std::shared_ptr<ShaderProgram> m_error_shader_program = std::make_shared<ShaderProgram>(
		"shaders/default/error.vert", "shaders/default/error.frag"
	);

	const std::shared_ptr<ShaderProgram> m_default_shader_program = std::make_shared<ShaderProgram>(
		"shaders/default/blinn_phong.vert", "shaders/default/blinn_phong.frag"
	);
	const std::shared_ptr<ShaderProgram> m_axes_shader_program = std::make_shared<ShaderProgram>(
		"shaders/default/axes.vert", "shaders/default/axes.frag"
	);
	const std::shared_ptr<ShaderProgram> preload_shader_programs [2] = {
		m_default_shader_program, m_axes_shader_program
	};
private:
	std::vector<std::shared_ptr<MeshNode>> m_mesh_nodes = {};
	mutable std::vector<ISceneObserver *> m_observers = {};
};

} // glpg
