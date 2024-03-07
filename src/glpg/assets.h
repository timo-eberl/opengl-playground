#pragma once

#include <string>

namespace glpg::assets {

// asset_path example: "shaders/default/shader.vert"
std::string read_text_file(const std::string& asset_path);

} // glpg::assets
