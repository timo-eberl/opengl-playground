#include "mesh_section.h"

using namespace glpg::meshes;

MeshSection::MeshSection(const std::shared_ptr<Geometry> geometry, const std::shared_ptr<Material> material)
	: m_geometry(geometry), m_material(material) {}

void MeshSection::draw() const { m_geometry->draw(); }
