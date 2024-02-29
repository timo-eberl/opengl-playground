#pragma once

#include <glm/glm.hpp>

#include "i_spatial.h"

namespace glpg {

class ICamera : ISpatial {
public:
	virtual ~ICamera() {};

	virtual glm::mat4 get_model_matrix() const override = 0;
	virtual void set_model_matrix(glm::mat4 model_matrix) override = 0;

	virtual glm::mat4 get_projection_matrix() const = 0;
};

} // glpg
