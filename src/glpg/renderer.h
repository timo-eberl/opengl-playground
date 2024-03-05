#pragma once

#include <glm/glm.hpp>

#include "scene.h"
#include "i_camera.h"

namespace glpg {

class Renderer {
public:
	bool auto_clear = true;
	void set_clear_color(glm::vec4 clear_color);

	void render(Scene &scene, const ICamera &camera);

	void clear();
	void clear_color();
	void clear_color(glm::vec4 clear_color);
	void clear_depth();
private:
	glm::vec4 m_clear_color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
};

} // glpg
