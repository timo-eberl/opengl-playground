#pragma once

#include "scene.h"

namespace glpg {

class AxesDrawer {
public:
	AxesDrawer();
	~AxesDrawer();
	// forbid copying, because it would be probably not what we want
	AxesDrawer(const AxesDrawer&) = delete;
	AxesDrawer &operator=(const AxesDrawer&) = delete;

	void draw(Scene &scene);
private:
	ShaderProgram m_shader_program = { "shaders/axes/shader.vert", "shaders/axes/shader.frag" };
	GLuint m_vertex_array = 0;
	GLuint m_indices_buffer = 0;
	GLuint m_positions_buffer = 0;
	GLuint m_color_buffer = 0;
};

} // glpg
