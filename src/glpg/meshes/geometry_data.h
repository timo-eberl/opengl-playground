#pragma once

#include <vector>
#include <cstdint>

#include <glm/glm.hpp>

namespace glpg::meshes {

struct GeometryData {
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;

	std::vector<uint32_t> indices;
};

} // glpg::meshes
