#pragma once

#include <vector>
#include <cstdint>

namespace glpg::meshes {

struct GeometryData {
	std::vector<float> vertices;

	unsigned int vertices_num_components;

	unsigned int vertices_position_num_components;
	unsigned int vertices_position_offset;

	unsigned int vertices_normal_num_components;
	unsigned int vertices_normal_offset;

	unsigned int vertices_uv_num_components;
	unsigned int vertices_uv_offset;

	std::vector<uint32_t> indices;
};

} // glpg::meshes
