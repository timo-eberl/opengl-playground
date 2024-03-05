#pragma once

#include "meshes.h"

namespace glpg::gltf_importer {

std::vector<meshes::MeshNode> import(const std::string& path);

} // glpg::gltf_importer
