#include "gltf_importer.h"

#include <cgltf.h>
#include <glm/glm.hpp>

#include <iostream>
#include <cassert>

#define ASSETS_DIR _ASSETS_DIR

using namespace glpg;

// declare "private" functions here instead of in the header
namespace glpg::gltf_importer {

void add_mesh_from_node(cgltf_node *node, meshes::Mesh &output_mesh);
void add_all_meshes_from_node_recursive(cgltf_node *node, meshes::Mesh &output_mesh);
bool is_primitive_valid(cgltf_primitive &primitive, cgltf_node *node);
void extract_attributes(
	cgltf_primitive &primitive,
	cgltf_accessor **pos_attribute, cgltf_accessor **normal_attribute, cgltf_accessor **uv_attribute
);
std::string result_to_string(cgltf_result result);

} // glpg::gltf_importer

meshes::Mesh gltf_importer::import(const std::string& path) {
	std::string full_path = ASSETS_DIR + path;
	cgltf_options options = {};
	cgltf_data* data = nullptr;
	// parse gltf or glb file
	auto parse_result = cgltf_parse_file(&options, full_path.c_str(), &data);
	// load binary buffers
	auto load_buffers_result = cgltf_load_buffers(&options, data, full_path.c_str());

	// treat the whole model as it would be a single mesh with multiple sections
	auto mesh = meshes::Mesh();

	if (parse_result != cgltf_result_success) {
		std::cerr << "\033[1;31m" // font color red
			<< "Importing glTF failed: " << result_to_string(parse_result) << ")\n\n"
			<< "\033[1;0m"; // reset styling
		return mesh;
	}
	else if (load_buffers_result != cgltf_result_success) {
		std::cerr << "\033[1;31m" // font color red
			<< "Importing glTF failed: " << result_to_string(load_buffers_result) << ")\n\n"
			<< "\033[1;0m"; // reset styling
		return mesh;
	}

	for (size_t i = 0; i < data->scene->nodes_count; i++) {
		auto node = data->scene->nodes[i];
		add_all_meshes_from_node_recursive(node, mesh);
	}
	cgltf_free(data);

	return mesh;
}

void gltf_importer::add_all_meshes_from_node_recursive(cgltf_node *node, meshes::Mesh &output_mesh) {
	if (node->mesh) {
		add_mesh_from_node(node, output_mesh);
	}
	for (size_t i = 0; i < node->children_count; i++) {
		add_all_meshes_from_node_recursive(node->children[i], output_mesh);
	}
}

void gltf_importer::add_mesh_from_node(cgltf_node *node, meshes::Mesh &output_mesh) {
	float mat4 [16];
	cgltf_node_transform_world(node, mat4);
	const auto node_world_matrix = glm::mat4(
		mat4[ 0], mat4[ 1], mat4[ 2], mat4[ 3],
		mat4[ 4], mat4[ 5], mat4[ 6], mat4[ 7],
		mat4[ 8], mat4[ 9], mat4[10], mat4[11],
		mat4[12], mat4[13], mat4[14], mat4[15]
	);

	for (size_t i = 0; i < node->mesh->primitives_count; i++) {
		auto primitive = node->mesh->primitives[i];

		if (!is_primitive_valid(primitive, node)) continue;

		auto geometry_data = glpg::meshes::GeometryData();

		// load index data from storage buffer into geometry data
		switch (primitive.indices->component_type) {
			case cgltf_component_type_r_16u: { // unsigned short / uint16_t
				// read uint16_t values
				uint16_t indices_buffer [primitive.indices->count];
				cgltf_accessor_unpack_indices(
					primitive.indices, indices_buffer, sizeof(uint16_t), primitive.indices->count
				);
				// insert into geometry data -> automatically convert to uint32_t
				geometry_data.indices.reserve(primitive.indices->count);
				for (const auto & index : indices_buffer) { geometry_data.indices.push_back(index); }
			} break;
			case cgltf_component_type_r_32u: { // unsigned int / uint32_t
				geometry_data.indices.resize(primitive.indices->count);
				// directly copy the indices data into the vector
				cgltf_accessor_unpack_indices(
					primitive.indices, geometry_data.indices.data(), sizeof(uint32_t), primitive.indices->count
				);
			} break;
		}

		cgltf_accessor *pos_attribute; cgltf_accessor *normal_attribute; cgltf_accessor *uv_attribute;
		extract_attributes(primitive, &pos_attribute, &normal_attribute, &uv_attribute);

		const auto vertices_count = pos_attribute->count;
		assert(vertices_count == normal_attribute->count);
		assert(vertices_count == uv_attribute->count);

		// load attribute data from storage buffer into geometry data
		geometry_data.positions.resize(vertices_count);
		geometry_data.normals.resize(vertices_count);
		geometry_data.uvs.resize(vertices_count);
		cgltf_accessor_unpack_floats(
			pos_attribute, reinterpret_cast<float *>(geometry_data.positions.data()), 3 * vertices_count
		);
		cgltf_accessor_unpack_floats(
			normal_attribute, reinterpret_cast<float *>(geometry_data.normals.data()), 3 * vertices_count
		);
		cgltf_accessor_unpack_floats(
			uv_attribute, reinterpret_cast<float *>(geometry_data.uvs.data()), 2 * vertices_count
		);

		output_mesh.sections.push_back(meshes::MeshSection(
			std::make_shared<meshes::Geometry>(std::move(geometry_data)),
			nullptr
		));
	}
}

void gltf_importer::extract_attributes(
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

bool gltf_importer::is_primitive_valid(cgltf_primitive &primitive, cgltf_node *node) {
	if (primitive.type != cgltf_primitive_type_triangles) {
		std::cerr << "\033[1;31m" // font color red
			<< "Unsupported mesh type. Only meshes of type cgltf_primitive_type_triangles are supported ("
			<< node->name << '.' << node->mesh->name << ")\n\n"
			<< "\033[1;0m"; // reset styling
		return false;
	}
	
	if (!primitive.indices) {
		std::cerr << "\033[1;31m" // font color red
			<< "Non-indexed meshes are not supported ("
			<< node->name << '.' << node->mesh->name << ")\n\n"
			<< "\033[1;0m"; // reset styling
		return false;
	}
	
	if (primitive.indices->component_type != cgltf_component_type_r_16u // unsigned short
		&& primitive.indices->component_type != cgltf_component_type_r_32u // unsigned int
	) {
		std::cerr << "\033[1;31m" // font color red
			<< "Index format other than unsigned short or unsigned int is not supported.\n\n"
			<< "\033[1;0m"; // reset styling
		return false;
	}

	cgltf_accessor *pos_attribute; cgltf_accessor *normal_attribute; cgltf_accessor *uv_attribute;
	extract_attributes(primitive, &pos_attribute, &normal_attribute, &uv_attribute);
	if (pos_attribute->component_type != cgltf_component_type_r_32f ||
		normal_attribute->component_type != cgltf_component_type_r_32f ||
		uv_attribute->component_type != cgltf_component_type_r_32f
	) {
		std::cerr << "\033[1;31m" // font color red
			<< "Unsupported attribute type. Only 32 bit floats are supported.\n\n"
			<< "\033[1;0m"; // reset styling
		return false;
	}
	if (pos_attribute->type != cgltf_type_vec3) {
		std::cerr << "\033[1;31m" // font color red
			<< "Position attribute must be a vector 3..\n\n"
			<< "\033[1;0m"; // reset styling
		return false;
	}
	if (normal_attribute->type != cgltf_type_vec3) {
		std::cerr << "\033[1;31m" // font color red
			<< "Normal attribute must be a vector 3..\n\n"
			<< "\033[1;0m"; // reset styling
		return false;
	}
	if (uv_attribute->type != cgltf_type_vec2) {
		std::cerr << "\033[1;31m" // font color red
			<< "Normal attribute must be a vector 2..\n\n"
			<< "\033[1;0m"; // reset styling
		return false;
	}

	return true;
}

std::string gltf_importer::result_to_string(cgltf_result result) {
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
