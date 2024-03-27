#include "camera_viewport_controls.h"

#include <glm/gtc/matrix_transform.hpp>

using ron::CameraViewportControls;

CameraViewportControls::CameraViewportControls(const glm::vec2 initial_rotation) : m_rotation(initial_rotation) {}

void CameraViewportControls::scroll_callback(ScrollDirection scroll_direction) {
	switch (scroll_direction) {
	case UP:
		m_camera_distance *= 0.8f;
		m_dirty = true;
		break;
	case DOWN:
		m_camera_distance *= 1.2f;
		m_dirty = true;
		break;
	}
}

void CameraViewportControls::set_target(const glm::vec3 target) {
	m_target = target;
	m_dirty = true;
}

void CameraViewportControls::update(GLFWwindow& window, ICamera &camera) {
	const bool shift_down = glfwGetKey(&window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
	const bool mouse_down = glfwGetMouseButton(&window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS;
	double x; double y; glfwGetCursorPos(&window, &x, &y);
	const auto mouse_pos = glm::vec2(x, y);
	const auto mouse_movement = glm::vec2(
		(mouse_pos.x - m_previous_mouse_pos.x),
		(mouse_pos.y - m_previous_mouse_pos.y)
	);

	ControlMode control_mode = IDLE;
	if (m_previous_control_mode == ORBIT && mouse_down) {
		control_mode = ORBIT;
	}
	else if (m_previous_control_mode == PAN && mouse_down) {
		control_mode = PAN;
	}
	else if (shift_down && mouse_down) {
		control_mode = PAN;
	}
	else if (!shift_down && mouse_down) {
		control_mode = ORBIT;
	}

	switch (control_mode) {
		case ORBIT: {
			m_rotation -= glm::vec2(mouse_movement.y, mouse_movement.x) * mouse_orbit_sensitivity * 0.01f;
			m_dirty = true;
		} break;
		case PAN: {
			const auto movement_2d = mouse_movement * m_camera_distance * mouse_pan_sensitivity * 0.00078f;
			const auto movement_3d_local = glm::vec3(-movement_2d.x, movement_2d.y, 0.0f);
			const auto movement_3d_world = glm::mat3(camera.get_model_matrix()) * movement_3d_local;
			m_target += movement_3d_world;
			m_dirty = true;
		} break;
		case IDLE: break;
	}

	m_previous_control_mode = control_mode;
	m_previous_mouse_pos = mouse_pos;

	if (!m_dirty) return;

	glm::mat4 cam_model_matrix = glm::identity<glm::mat4>();
	cam_model_matrix = glm::translate(cam_model_matrix, m_target);
	cam_model_matrix = glm::rotate(
		cam_model_matrix, m_rotation.y, glm::vec3(0.0f, 1.0f, 0.0f)
	);
	cam_model_matrix = glm::rotate(
		cam_model_matrix, m_rotation.x, glm::vec3(1.0f, 0.0f, 0.0f)
	);
	cam_model_matrix = glm::translate(cam_model_matrix, glm::vec3(0.0f, 0.0f, m_camera_distance));

	camera.set_model_matrix(cam_model_matrix);
	m_dirty = false;
}
