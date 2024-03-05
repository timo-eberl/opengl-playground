#include "glpg/meshes.h"

using namespace glpg::meshes;

void Mesh::draw() const {
	for (const auto & section : sections) { section.draw(); }
}

void Mesh::send_to_gpu() {
	for (auto & section : sections) { section.send_to_gpu(); }
}
