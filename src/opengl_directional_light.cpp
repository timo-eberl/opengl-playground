#include "opengl_rendering.h"

using namespace ron;

OpenGLDirectionalLightGPUData ron::opengl_setup_dir_light(const DirectionalLight &dir_light) {
	if (!dir_light.shadow.enabled) {
		return {};
	}

	OpenGLDirectionalLightGPUData gpu_data;

	glGenFramebuffers(1, &gpu_data.shadow_map_framebuffer);

	glGenTextures(1, &gpu_data.shadow_map);
	glBindTexture(GL_TEXTURE_2D, gpu_data.shadow_map);

	// set texture wrap to clamp and set a white border, so areas that are not covered by
	// the shadow map are not shadowed
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat border_color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		dir_light.shadow.map_size.x, dir_light.shadow.map_size.y, 0, GL_DEPTH_COMPONENT,
		GL_FLOAT, NULL
	);

	glBindFramebuffer(GL_FRAMEBUFFER, gpu_data.shadow_map_framebuffer);
	glFramebufferTexture2D(
		GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gpu_data.shadow_map, 0
	);
	// We only need the depth information when rendering the scene from the light's perspective
	// so there is no need for a color buffer
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	return gpu_data;
}

void ron::opengl_release_dir_light(OpenGLDirectionalLightGPUData &gpu_data) {
	glDeleteTextures(1, &gpu_data.shadow_map);
	glDeleteFramebuffers(1, &gpu_data.shadow_map_framebuffer);
}
