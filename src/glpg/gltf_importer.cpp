#include "gltf_importer.h"

#include <cgltf.h>
#include <glm/glm.hpp>

#include <iostream>
#include <cassert>

#define ASSETS_DIR _ASSETS_DIR

using namespace glpg;

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

	if (parse_result == cgltf_result_success) {
		for (size_t i = 0; i < data->scene->nodes_count; i++) {
			// todo: traverse children

			auto node = data->scene->nodes[i];
			if (!node->mesh) continue;

			for (size_t i = 0; i < node->mesh->primitives_count; i++) {
				auto primitive = node->mesh->primitives[i];
				if (primitive.type != cgltf_primitive_type_triangles) {
					std::cerr << "\033[1;31m" // font color red
						<< "Only meshes of type cgltf_primitive_type_triangles are supported ("
						<< node->name << '.' << node->mesh->name << ")\n\n"
						<< "\033[1;0m"; // reset styling
					continue;
				}
				else if (!primitive.indices) {
					std::cerr << "\033[1;31m" // font color red
						<< "Non-indexed meshes are not supported ("
						<< node->name << '.' << node->mesh->name << ")\n\n"
						<< "\033[1;0m"; // reset styling
					continue;
				}
				else if (primitive.indices->component_type != cgltf_component_type_r_16u // unsigned short
					&& primitive.indices->component_type != cgltf_component_type_r_32u // unsigned int
				) {
					std::cerr << "\033[1;31m" // font color red
						<< "Index format other than unsigned short or unsigned int is not supported.\n\n"
						<< "\033[1;0m"; // reset styling
					continue;
				}

				auto geometry_data = glpg::meshes::GeometryData();

				// set indices
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

				cgltf_accessor * pos_attribute;
				cgltf_accessor * normal_attribute;
				cgltf_accessor * uv_attribute;
				for (size_t i = 0; i < primitive.attributes_count; i++) {
					const auto name = std::string(primitive.attributes[i].name);
					if (name == std::string("POSITION")) {
						pos_attribute = primitive.attributes[i].data;
					}
					else if (name == std::string("NORMAL")) {
						normal_attribute = primitive.attributes[i].data;
					}
					else if (name == std::string("TEXCOORD_0")) {
						uv_attribute = primitive.attributes[i].data;
					}
				}
				const auto vertices_count = pos_attribute->count;
				assert(vertices_count == normal_attribute->count);
				assert(vertices_count == uv_attribute->count);

				const auto pos_num_components = cgltf_num_components(pos_attribute->type);
				const auto positions_buffer_float_count = pos_num_components * vertices_count;
				float positions_buffer [positions_buffer_float_count];
				cgltf_accessor_unpack_floats(
					pos_attribute, positions_buffer, positions_buffer_float_count
				);

				const auto normal_num_components = cgltf_num_components(normal_attribute->type);
				const auto normal_buffer_float_count = normal_num_components * vertices_count;
				float normal_buffer [normal_buffer_float_count];
				cgltf_accessor_unpack_floats(
					normal_attribute, normal_buffer, normal_buffer_float_count
				);

				const auto uv_num_components = cgltf_num_components(uv_attribute->type);
				const auto uv_buffer_float_count = uv_num_components * vertices_count;
				float uv_buffer [uv_buffer_float_count];
				cgltf_accessor_unpack_floats(
					uv_attribute, uv_buffer, uv_buffer_float_count
				);

				geometry_data.vertices_num_components
					= pos_num_components + normal_num_components + uv_num_components;

				geometry_data.vertices_position_num_components = pos_num_components;
				geometry_data.vertices_position_offset = 0;

				geometry_data.vertices_normal_num_components = normal_num_components;
				geometry_data.vertices_normal_offset = pos_num_components;

				geometry_data.vertices_uv_num_components = uv_num_components;
				geometry_data.vertices_uv_offset = pos_num_components + normal_num_components;

				geometry_data.vertices.resize(geometry_data.vertices_num_components * vertices_count);
				for (size_t vertex_idx = 0; vertex_idx < vertices_count; vertex_idx++) {
					// get pointer into vector
					auto insert_vert_here = &geometry_data.vertices[
						vertex_idx * geometry_data.vertices_num_components
					];

					auto insert_pos_here = insert_vert_here + geometry_data.vertices_position_offset;
					cgltf_accessor_read_float(pos_attribute, vertex_idx, insert_pos_here, pos_num_components);

					auto insert_normal_here = insert_vert_here + geometry_data.vertices_normal_offset;
					cgltf_accessor_read_float(normal_attribute, vertex_idx, insert_normal_here, normal_num_components);

					auto insert_uv_here = insert_vert_here + geometry_data.vertices_uv_offset;
					cgltf_accessor_read_float(uv_attribute, vertex_idx, insert_uv_here, uv_num_components);
				}
				mesh.sections.push_back(meshes::MeshSection(
					std::make_shared<meshes::Geometry>(std::move(geometry_data)),
					nullptr
				));
			}
		}

		cgltf_free(data);
	}
	else {
		std::string result_str;
		switch (parse_result) {
			case cgltf_result_success: result_str = "cgltf_result_success"; break;
			case cgltf_result_data_too_short: result_str = "cgltf_result_data_too_short"; break;
			case cgltf_result_unknown_format: result_str = "cgltf_result_unknown_format"; break;
			case cgltf_result_invalid_json: result_str = "cgltf_result_invalid_json"; break;
			case cgltf_result_invalid_gltf: result_str = "cgltf_result_invalid_gltf"; break;
			case cgltf_result_invalid_options: result_str = "cgltf_result_invalid_options"; break;
			case cgltf_result_file_not_found: result_str = "cgltf_result_file_not_found"; break;
			case cgltf_result_io_error: result_str = "cgltf_result_io_error"; break;
			case cgltf_result_out_of_memory: result_str = "cgltf_result_out_of_memory"; break;
			case cgltf_result_legacy_gltf: result_str = "cgltf_result_legacy_gltf"; break;
			case cgltf_result_max_enum: result_str = "cgltf_result_max_enum"; break;
			default: result_str = "unknown error"; break;
		}
		std::cerr << "\033[1;31m" // font color red
			<< "Importing glTF failed: " << result_str << ")\n\n"
			<< "\033[1;0m"; // reset styling
	}
	return mesh;
}
