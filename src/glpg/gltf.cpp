#include "gltf.h"

#include <cgltf.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cassert>

#include "log.h"

#define ASSETS_DIR _ASSETS_DIR

using namespace glpg;

static void extract_attributes(
	cgltf_primitive &primitive,
	cgltf_accessor **pos_attribute, cgltf_accessor **normal_attribute, cgltf_accessor **uv_attribute
) {
	for (size_t i = 0; i < primitive.attributes_count; i++) {
		const auto name = std::string(primitive.attributes[i].name);
		if (name == std::string("POSITION")) {
			*pos_attribute = primitive.attributes[i].data;
		}
		else if (name == std::string("NORMAL")) {
			*normal_attribute = primitive.attributes[i].data;
		}
		else if (name == std::string("TEXCOORD_0")) {
			*uv_attribute = primitive.attributes[i].data;
		}
	}
}

static bool is_primitive_valid(cgltf_primitive &primitive, cgltf_node *node, std::vector<std::string> &unsupported) {
	if (primitive.type != cgltf_primitive_type_triangles) {
		unsupported.push_back("Mesh type other than primitive triangles");
	}
	if (!primitive.indices) {
		unsupported.push_back("Non-indexed mesh");
	}
	if (primitive.indices->component_type != cgltf_component_type_r_16u // unsigned short
		&& primitive.indices->component_type != cgltf_component_type_r_32u // unsigned int
	) {
		unsupported.push_back("Index format other than unsigned short or unsigned int");
	}

	cgltf_accessor *pos_attribute = nullptr;
	cgltf_accessor *normal_attribute = nullptr; // optional
	cgltf_accessor *uv_attribute = nullptr; // optional
	extract_attributes(primitive, &pos_attribute, &normal_attribute, &uv_attribute);
	if (pos_attribute->component_type != cgltf_component_type_r_32f ||
		(normal_attribute && normal_attribute->component_type != cgltf_component_type_r_32f) ||
		(uv_attribute && uv_attribute->component_type != cgltf_component_type_r_32f)
	) {
		unsupported.push_back("Attribute type other than 32 bit float");
	}
	if (pos_attribute->type != cgltf_type_vec3) {
		unsupported.push_back("Position attribute with a number of components other than 3");
	}
	if (normal_attribute && normal_attribute->type != cgltf_type_vec3) {
		unsupported.push_back("Normal attribute with a number of components other than 3");
	}
	if (uv_attribute && uv_attribute->type != cgltf_type_vec2) {
		unsupported.push_back("Texture Coordinate attribute with a number of components other than 2");
	}

	return unsupported.size() == 0;
}

static void add_mesh_from_node(cgltf_node *node, Mesh &out_mesh, std::vector<std::string> &unsupported) {
	for (size_t i = 0; i < node->mesh->primitives_count; i++) {
		auto primitive = node->mesh->primitives[i];

		std::vector<std::string> unsupported_new;
		if (!is_primitive_valid(primitive, node, unsupported_new)) {
			for (auto &new_error : unsupported_new) {
				new_error = new_error + " (" + node->name + "." + node->mesh->name + ")";
			}
			unsupported.insert(unsupported.end(), unsupported_new.begin(), unsupported_new.end());
			continue;
		}

		auto geometry = glpg::Geometry();

		// load index data from storage buffer into geometry data
		switch (primitive.indices->component_type) {
			case cgltf_component_type_r_16u: { // unsigned short / uint16_t
				// read uint16_t values
				uint16_t indices_buffer [primitive.indices->count];
				cgltf_accessor_unpack_indices(
					primitive.indices, indices_buffer, sizeof(uint16_t), primitive.indices->count
				);
				// insert into geometry data -> automatically convert to uint32_t
				geometry.indices.reserve(primitive.indices->count);
				for (const auto & index : indices_buffer) { geometry.indices.push_back(index); }
			} break;
			case cgltf_component_type_r_32u: { // unsigned int / uint32_t
				geometry.indices.resize(primitive.indices->count);
				// directly copy the indices data into the vector
				cgltf_accessor_unpack_indices(
					primitive.indices, geometry.indices.data(), sizeof(uint32_t), primitive.indices->count
				);
			} break;
		}

		cgltf_accessor *pos_attribute = nullptr;
		cgltf_accessor *normal_attribute = nullptr; // optional
		cgltf_accessor *uv_attribute = nullptr; // optional
		extract_attributes(primitive, &pos_attribute, &normal_attribute, &uv_attribute);

		assert(pos_attribute); // position attribute must exist
		const auto vertices_count = pos_attribute->count;
		// normal and uvs are optional
		assert(!normal_attribute || vertices_count == normal_attribute->count);
		assert(!uv_attribute || vertices_count == uv_attribute->count);

		// load attribute data from storage buffer into geometry data
		geometry.positions.resize(vertices_count);
		cgltf_accessor_unpack_floats(
			pos_attribute, reinterpret_cast<float *>(geometry.positions.data()), 3 * vertices_count
		);
		if (normal_attribute) {
			geometry.normals.resize(vertices_count);
			cgltf_accessor_unpack_floats(
				normal_attribute, reinterpret_cast<float *>(geometry.normals.data()), 3 * vertices_count
			);
		}
		if (uv_attribute) {
			geometry.uvs.resize(vertices_count);
			cgltf_accessor_unpack_floats(
				uv_attribute, reinterpret_cast<float *>(geometry.uvs.data()), 2 * vertices_count
			);
		}

		out_mesh.sections.push_back(MeshSection(
			std::make_shared<Geometry>(std::move(geometry)),
			nullptr
		));
	}
}

static void add_all_meshes_from_node_recursive(cgltf_node *node, Scene &scene, std::vector<std::string> &unsupported) {
	if (node->mesh) {
		auto node_world_matrix = glm::identity<glm::mat4>();
		cgltf_node_transform_world(node, reinterpret_cast<float *>(&node_world_matrix));

		auto mesh = std::make_shared<Mesh>();
		add_mesh_from_node(node, *mesh, unsupported);
		scene.add(std::make_shared<MeshNode>(mesh, node_world_matrix));
	}
	for (size_t i = 0; i < node->children_count; i++) {
		add_all_meshes_from_node_recursive(node->children[i], scene, unsupported);
	}
}

static std::string result_to_string(cgltf_result result) {
	switch (result) {
		case cgltf_result_success: return "cgltf_result_success";
		case cgltf_result_data_too_short: return "cgltf_result_data_too_short";
		case cgltf_result_unknown_format: return "cgltf_result_unknown_format";
		case cgltf_result_invalid_json: return "cgltf_result_invalid_json";
		case cgltf_result_invalid_gltf: return "cgltf_result_invalid_gltf";
		case cgltf_result_invalid_options: return "cgltf_result_invalid_options";
		case cgltf_result_file_not_found: return "cgltf_result_file_not_found";
		case cgltf_result_io_error: return "cgltf_result_io_error";
		case cgltf_result_out_of_memory: return "cgltf_result_out_of_memory";
		case cgltf_result_legacy_gltf: return "cgltf_result_legacy_gltf";
		case cgltf_result_max_enum: return "cgltf_result_max_enum";
		default: return "unknown error";
	}
}

Scene gltf::import(const std::string& path) {
	std::string full_path = ASSETS_DIR + path;
	cgltf_options options = {};
	cgltf_data* data = nullptr;
	// parse gltf or glb file
	auto parse_result = cgltf_parse_file(&options, full_path.c_str(), &data);
	// load binary buffers
	auto load_buffers_result = cgltf_load_buffers(&options, data, full_path.c_str());

	Scene scene = {};

	if (parse_result != cgltf_result_success) {
		log::error("Importing glTF failed: " + result_to_string(parse_result) + " (" + path + ")");
		return scene;
	}
	else if (load_buffers_result != cgltf_result_success) {
		log::error("Loading glTF buffers failed: " + result_to_string(load_buffers_result) + " (" + path + ")");
		return scene;
	}

	std::vector<std::string> unsupported_features = {};
	for (size_t i = 0; i < data->scene->nodes_count; i++) {
		auto node = data->scene->nodes[i];
		add_all_meshes_from_node_recursive(node, scene, unsupported_features);
	}

	if (unsupported_features.size() > 0) {
		log::error("Incomplete glTF import. " + path + " uses unsupported features:");
		for (const auto &error : unsupported_features) {
			log::error("\t- " + error);
		}
	}
	else {
		log::success("glTF imported successfully (" + path + ")");
	}

	cgltf_free(data);

	return scene;
}
