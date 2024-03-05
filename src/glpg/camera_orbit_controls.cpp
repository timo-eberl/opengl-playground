#include "camera_orbit_controls.h"

#include <glm/gtc/matrix_transform.hpp>

using glpg::CameraOrbitControls;

CameraOrbitControls::CameraOrbitControls(const std::weak_ptr<ICamera> camera, const glm::vec2 initial_rotation)
	: m_camera(camera), m_rotation(initial_rotation) {}

void CameraOrbitControls::scroll_callback(ScrollDirection scroll_direction) {
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

void CameraOrbitControls::set_target(const glm::vec3 target) {
	m_target = target;
	m_dirty = true;
}

void CameraOrbitControls::update(GLFWwindow& window) {
	const bool mouse_down = glfwGetMouseButton(&window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS;
	if (mouse_down) {
		double x; double y; glfwGetCursorPos(&window, &x, &y);
		const auto mouse_pos = glm::vec2(x, y);

		if (!m_previous_mouse_down) {
			m_previous_mouse_pos = mouse_pos;
		}

		m_rotation.y -= (mouse_pos.x - m_previous_mouse_pos.x) * mouse_sensitivity.x * 0.01f;
		m_rotation.x -= (mouse_pos.y - m_previous_mouse_pos.y) * mouse_sensitivity.y * 0.01f;

		m_dirty = true;
		m_previous_mouse_pos = mouse_pos;
	}
	m_previous_mouse_down = mouse_down;

	if (!m_dirty) return;

	if (const auto sp_cam = m_camera.lock()) {
		glm::mat4 cam_model_matrix = glm::identity<glm::mat4>();
		cam_model_matrix = glm::translate(cam_model_matrix, m_target);
		cam_model_matrix = glm::rotate(
			cam_model_matrix, m_rotation.y, glm::vec3(0.0f, 1.0f, 0.0f)
		);
		cam_model_matrix = glm::rotate(
			cam_model_matrix, m_rotation.x, glm::vec3(1.0f, 0.0f, 0.0f)
		);
		cam_model_matrix = glm::translate(cam_model_matrix, glm::vec3(0.0f, 0.0f, m_camera_distance));

		sp_cam->set_model_matrix(cam_model_matrix);
		m_dirty = false;
	}
}
