#pragma once

#include <glm/glm.hpp>

namespace ron {

struct Shadow {
	glm::uvec2 map_size = glm::uvec2(1024);
	// the size of the area shadows are visible in (side length of the square area)
	// 100 means 50 from the camera position to the left, right, forward, backward
	float frustum_size = 100.0f;

	float near = 0.5f;
	float far = 500.0f;
	float bias = 0.05f;

	bool enabled = false;
};

struct DirectionalLight {
	Shadow shadow;

	glm::vec3 world_direction = glm::normalize(glm::vec3(4.1f, 5.9f, -1.0f));
	glm::vec3 world_position = glm::vec3(0.0f, 10.0f, 0.0f);

	float intensity = 1.0f;
};

} // ron
