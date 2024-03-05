#include "glpg/meshes.h"

using namespace glpg::meshes;

MeshSection::MeshSection(const std::shared_ptr<RenderGeometry> geometry, const std::shared_ptr<Material> material)
	: m_geometry(geometry), m_material(material) {}

void MeshSection::draw() const { m_geometry->draw(); }

void MeshSection::send_to_gpu() { m_geometry->send_to_gpu(); }
