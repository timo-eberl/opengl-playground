#pragma once

#include <memory>

#include "shader_program.h"
#include "uniforms.h"

namespace glpg {

struct Material {
	Uniforms uniforms = {};
	std::shared_ptr<ShaderProgram> shader_program = {};
};

} // glpg
