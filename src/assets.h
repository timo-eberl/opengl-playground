#pragma once

#include <string>
#include <memory>

#include "shader_program.h"

namespace ron::assets {

// asset_path example: "shaders/fancy_shader.vert"
std::string read_text_file(const std::string& asset_path);

std::shared_ptr<ShaderProgram> load_shader_program(
	const std::string &vertex_shader_asset_path, const std::string &fragment_shader_asset_path
);

void reload_shader_programs();

} // ron::assets
