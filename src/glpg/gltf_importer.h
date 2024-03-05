#pragma once

#include <string>

#include "meshes/mesh.h"

namespace glpg::gltf_importer {

meshes::Mesh import(const std::string& path);

} // glpg::gltf_importer
