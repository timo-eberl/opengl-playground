#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h> // include glfw after glad

#include <memory>

#include "i_camera.h"

namespace ron {

class CameraViewportControls {
public:
	CameraViewportControls(const glm::vec2 initial_rotation = glm::vec2(0.0f));
	void update(GLFWwindow& window, ICamera &camera); // needs to be called every frame
	void set_target(const glm::vec3 target);
	glm::vec3 get_target() const;
	float mouse_orbit_sensitivity = 1.0f;
	float mouse_pan_sensitivity = 1.0f;

	enum ScrollDirection { UP, DOWN };
	void scroll_callback(ScrollDirection scroll_direction);
private:
	enum ControlMode { IDLE, PAN, ORBIT };

	glm::vec3 m_target = glm::vec3(0.0f);
	glm::vec2 m_rotation;
	glm::vec2 m_previous_mouse_pos = glm::vec2(0.0f);
	float m_camera_distance = 24.0f;
	ControlMode m_previous_control_mode = IDLE;
	bool m_dirty = true;
};

} // ron
