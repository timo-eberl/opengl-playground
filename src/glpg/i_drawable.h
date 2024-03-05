#pragma once

#include "uniforms.h"

namespace glpg {

class IDrawable {
public:
	virtual ~IDrawable() {};

	virtual void setup_drawing() = 0;
	virtual void draw(Uniforms &uniforms) const = 0;
};

} // glpg
