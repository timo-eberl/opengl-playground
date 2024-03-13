#pragma once

#include <memory>

#include "shader_program.h"
#include "uniforms.h"

namespace glpg {

struct Material {
	enum CullingMode { NONE, FRONT, BACK };

	Uniforms uniforms = {};
	std::shared_ptr<ShaderProgram> shader_program = {};
	CullingMode culling_mode = BACK;
};

} // glpg
