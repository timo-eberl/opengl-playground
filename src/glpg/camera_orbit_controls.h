#pragma once

#include <glad/glad.h>
// include glfw after glad
#include <GLFW/glfw3.h>

#include <memory>

#include "i_camera.h"

namespace glpg {

class CameraOrbitControls {
public:
	CameraOrbitControls(
		const std::weak_ptr<ICamera> camera, const glm::vec2 initial_rotation = glm::vec2(0.0f)
	);
	void update(GLFWwindow& window); // needs to be called every frame
	void set_target(const glm::vec3 target);
	glm::vec2 mouse_sensitivity = glm::vec2(1.0f);

	enum ScrollDirection { UP, DOWN };
	void scroll_callback(ScrollDirection scroll_direction);
private:
	glm::vec3 m_target = glm::vec3(0.0f);
	glm::vec2 m_rotation;
	glm::vec2 m_previous_mouse_pos = glm::vec2(0.0f);
	const std::weak_ptr<ICamera> m_camera;
	const std::weak_ptr<GLFWwindow> m_window;
	float m_camera_distance = 18.0f;
	bool m_dirty = true;
	bool m_previous_mouse_down = false;
};

} // glpg
