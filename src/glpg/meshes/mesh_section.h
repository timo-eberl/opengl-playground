#pragma once

#include "geometry.h"
#include "glpg/material.h"

namespace glpg::meshes {

struct MeshSection {
public:
	MeshSection(const std::shared_ptr<Geometry> geometry, const std::shared_ptr<Material> material);

	void draw() const;
private:
	const std::shared_ptr<Geometry> m_geometry;
	const std::shared_ptr<Material> m_material;
};

} // glpg::meshes
