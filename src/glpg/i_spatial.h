#pragma once

#include <glm/glm.hpp>

namespace glpg {

class ISpatial {
public:
	virtual ~ISpatial() {};

	virtual glm::mat4 get_model_matrix() const = 0;
	virtual void set_model_matrix(glm::mat4 model_matrix) = 0;
};

} // glpg
