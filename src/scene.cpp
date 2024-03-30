#include "scene.h"

#include <algorithm>

#include "assets.h"

using namespace ron;

static void set_light_uniforms(const DirectionalLight &light, Uniforms &uniforms) {
	uniforms["directional_light_world_direction"] = make_uniform(light.world_direction);
	uniforms["directional_light_intensity"] = make_uniform(light.intensity);
	uniforms["directional_light_shadow_enabled"] = make_uniform(static_cast<int>(light.shadow.enabled));
	uniforms["directional_light_shadow_bias"] = make_uniform(light.shadow.bias);
}

Scene::Scene() : default_material(std::make_shared<Material>()) {
	default_material->shader_program = assets::load_shader_program(
		"default/shaders/blinn_phong.vert", "default/shaders/blinn_phong.frag"
	);
	default_material->uniforms["albedo_tex"] = ron::make_uniform(
		assets::load_texture("default/textures/white.jpg")
	);
	default_material->uniforms["normal_tex"] = ron::make_uniform(assets::load_texture(
		"default/textures/normal.png",
		Texture::MetaData(Texture::Channels::AUTOMATIC, Texture::ColorSpace::NON_COLOR)
	));
	default_material->uniforms["metallic_roughness_tex"] = ron::make_uniform(assets::load_texture(
		"default/textures/white.jpg",
		Texture::MetaData(Texture::Channels::AUTOMATIC, Texture::ColorSpace::NON_COLOR)
	));
	default_material->uniforms["albedo_color"] = make_uniform(glm::vec4(1.0, 1.0, 1.0, 1.0));
	default_material->uniforms["metallic_factor"] = make_uniform(glm::vec1(0.0));
	default_material->uniforms["roughness_factor"] = make_uniform(glm::vec1(0.5));

	set_light_uniforms(*m_directional_light, global_uniforms);
	m_directional_light_update_count++;
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
	m_mesh_nodes.erase(
		std::remove_if(m_mesh_nodes.begin(), m_mesh_nodes.end(), is_equals), m_mesh_nodes.end()
	);
}

void Scene::set_directional_light(const DirectionalLight &directional_light) {
	*m_directional_light = directional_light;
	set_light_uniforms(*m_directional_light, global_uniforms);
	m_directional_light_update_count++;
}

std::shared_ptr<const DirectionalLight> Scene::get_directional_light() const {
	return m_directional_light;
}

unsigned int Scene::get_directional_light_update_count() const {
	return m_directional_light_update_count;
}
