#include "axes_drawer.h"

using namespace glpg;

AxesDrawer::AxesDrawer() {
	if (m_vertex_array != 0) return;

	static const GLint position_attrib_index = 0; // layout (location = 0)
	static const GLint color_attrib_index = 1; // layout (location = 1)

	// any subsequent vertex attribute calls and bind buffer calls
	// will be stored inside the vertex array
	glGenVertexArrays(1, &m_vertex_array);
	glBindVertexArray(m_vertex_array);

	// indices
	const GLuint indices_buffer [] = { 0,1,2,3,4,5 };
	glGenBuffers(1, &m_indices_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indices_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_buffer), indices_buffer, GL_STATIC_DRAW);

	// positions
	const GLfloat positions_buffer [] = {
		-10000.0f, 0.0f, 0.0f,    10000.0f, 0.0f, 0.0f, // x
		0.0f, -10000.0f, 0.0f,    0.0f, 10000.0f, 0.0f, // y
		0.0f, 0.0f, -10000.0f,    0.0f, 0.0f, 10000.0f, // z
	};
	glGenBuffers(1, &m_positions_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_positions_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions_buffer), positions_buffer, GL_STATIC_DRAW);
	glVertexAttribPointer(position_attrib_index, 3, GL_FLOAT, false, 3 * sizeof(GLfloat), static_cast<GLvoid*>(0));
	glEnableVertexAttribArray(position_attrib_index);

	// colors
	const GLfloat colors_buffer [] = {
		0.604f, 0.239f, 0.290f, // x
		0.604f, 0.239f, 0.290f, // x
		0.388f, 0.541f, 0.153f, // y
		0.388f, 0.541f, 0.153f, // y
		0.235f, 0.406f, 0.604f, // z
		0.235f, 0.406f, 0.604f, // z
	};
	glGenBuffers(1, &m_color_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_color_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors_buffer), colors_buffer, GL_STATIC_DRAW);
	glVertexAttribPointer(color_attrib_index, 3, GL_FLOAT, false, 3 * sizeof(GLfloat), static_cast<GLvoid*>(0));
	glEnableVertexAttribArray(color_attrib_index);

	// unbind buffers to avoid accidental modification
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

AxesDrawer::~AxesDrawer() {
	glDeleteBuffers(1, &m_positions_buffer);
	glDeleteBuffers(1, &m_indices_buffer);
	glDeleteBuffers(1, &m_color_buffer);

	glDeleteVertexArrays(1, &m_vertex_array);
}

void AxesDrawer::draw(Scene &scene) {
	assert(m_vertex_array != 0);

	glUseProgram(m_shader_program.get_id());

	scene.global_uniforms.apply_to_program(m_shader_program);

	glBindVertexArray(m_vertex_array);
	glLineWidth(2.0f);
	glDrawElements(GL_LINES, 6, GL_UNSIGNED_INT, 0);

	// unbind to avoid accidental modification
	glBindVertexArray(0);
	glUseProgram(0);
}
