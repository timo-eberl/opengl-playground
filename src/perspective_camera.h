#pragma once

#include <glm/glm.hpp>

#include "i_camera.h"

namespace ron {

class PerspectiveCamera : public ICamera {
public:
	PerspectiveCamera(
		const float fov = 45.0f, const float aspect_ratio = 1.0f,
		const float near_clipping_plane = 0.1f, const float far_clipping_plane = 1000.0f
	);
	~PerspectiveCamera() {};

	// ISpatial
	virtual glm::mat4 get_model_matrix() const override;
	virtual void set_model_matrix(glm::mat4 model_matrix) override;

	// ICamera
	virtual glm::mat4 get_projection_matrix() const override;

	void set_fov(const float fov);
	void set_aspect_ratio(const float aspect_ratio);
	void set_near_clipping_plane(const float near_clipping_plane);
	void set_far_clipping_plane(const float far_clipping_plane);
private:
	glm::mat4 m_model_matrix;
	glm::mat4 m_projection_matrix;
	float m_fov;
	float m_aspect_ratio;
	float m_near_clipping_plane;
	float m_far_clipping_plane;

	void update_projection_matrix();
};

} // ron
