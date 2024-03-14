#pragma once

#include <string>

namespace ron::assets {

// asset_path example: "shaders/fancy_shader.vert"
std::string read_text_file(const std::string& asset_path);

} // ron::assets
