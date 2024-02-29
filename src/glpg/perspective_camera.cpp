#include "perspective_camera.h"

#include <glm/gtc/matrix_transform.hpp>

using glpg::PerspectiveCamera;

PerspectiveCamera::PerspectiveCamera(
	const float fov, const float aspect_ratio,
	const float near_clipping_plane, const float far_clipping_plane
)
	: m_model_matrix(glm::identity<glm::mat4>()), m_projection_matrix(glm::identity<glm::mat4>()),
	m_fov(fov), m_aspect_ratio(aspect_ratio), m_near_clipping_plane(near_clipping_plane),
	m_far_clipping_plane(far_clipping_plane)
{
	update_projection_matrix();
}

glm::mat4 PerspectiveCamera::get_model_matrix() const {
	return m_model_matrix;
}

void PerspectiveCamera::set_model_matrix(glm::mat4 model_matrix) {
	m_model_matrix = model_matrix;
}

glm::mat4 PerspectiveCamera::get_projection_matrix() const {
	return m_projection_matrix;
}

void PerspectiveCamera::set_fov(const float fov) {
	m_fov = fov;
	update_projection_matrix();
}

void PerspectiveCamera::set_aspect_ratio(const float aspect_ratio) {
	m_aspect_ratio = aspect_ratio;
	update_projection_matrix();
}

void PerspectiveCamera::set_near_clipping_plane(const float near_clipping_plane) {
	m_near_clipping_plane = near_clipping_plane;
	update_projection_matrix();
}

void PerspectiveCamera::set_far_clipping_plane(const float far_clipping_plane) {
	m_far_clipping_plane = far_clipping_plane;
	update_projection_matrix();
}

void PerspectiveCamera::update_projection_matrix() {
	m_projection_matrix = glm::perspective(
		glm::radians(m_fov), m_aspect_ratio, m_near_clipping_plane, m_far_clipping_plane
	);
}
