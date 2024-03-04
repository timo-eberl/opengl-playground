#include "mesh.h"

using namespace glpg::meshes;

void Mesh::draw() const {
	for (const auto & section : sections) { section.draw(); }
}
