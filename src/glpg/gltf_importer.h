#pragma once

#include <string>

#include "meshes/mesh.h"

namespace glpg::gltf_importer {

meshes::Mesh import(const std::string& path);

void import(
		const std::string& path,
		void ** indices, int * indices_count, bool * indices_format_short,
		float ** vertices, int * vertices_count
);

} // glpg::gltf_importer
