#pragma once

#include "mesh_section.h"

namespace glpg::meshes {

class Mesh {
public:
	std::vector<MeshSection> sections;

	void draw() const;
};

} // glpg::meshes
