#include "opengl_rendering.h"

using namespace ron;

OpenGLGridRenderer::OpenGLGridRenderer() {
	if (m_vertex_array != 0) return;

	static const GLint position_attrib_index = 0; // layout (location = 0)
	static const GLint color_attrib_index = 1; // layout (location = 1)

	// any subsequent vertex attribute calls and bind buffer calls
	// will be stored inside the vertex array
	glGenVertexArrays(1, &m_vertex_array);
	glBindVertexArray(m_vertex_array);

	// positions
	auto positions_buffer = std::vector<glm::vec3>();
	positions_buffer.reserve(8 * m_distance);
	for (unsigned int i = 0; i < m_distance; i++) {
		positions_buffer.push_back(glm::vec3( m_distance, 0,  i));
		positions_buffer.push_back(glm::vec3(-m_distance, 0,  i));
		positions_buffer.push_back(glm::vec3( m_distance, 0, -i));
		positions_buffer.push_back(glm::vec3(-m_distance, 0, -i));
		positions_buffer.push_back(glm::vec3( i, 0,  m_distance));
		positions_buffer.push_back(glm::vec3( i, 0, -m_distance));
		positions_buffer.push_back(glm::vec3(-i, 0,  m_distance));
		positions_buffer.push_back(glm::vec3(-i, 0, -m_distance));
	}
	glGenBuffers(1, &m_positions_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_positions_buffer);
	glBufferData(GL_ARRAY_BUFFER,
		positions_buffer.size() * sizeof(glm::vec3),
		positions_buffer.data(), GL_STATIC_DRAW
	);
	glVertexAttribPointer(
		position_attrib_index, 3, GL_FLOAT, false, sizeof(glm::vec3), static_cast<GLvoid*>(0)
	);
	glEnableVertexAttribArray(position_attrib_index);

	// colors
	auto colors_buffer = std::vector<glm::vec3>();
	colors_buffer.reserve(8 * m_distance);
	for (size_t i = 0; i < m_distance; i++) {
		const auto brightness = (i % 10 == 0) ? 0.34f : 0.29f;
		colors_buffer.push_back(glm::vec3(brightness));
		colors_buffer.push_back(glm::vec3(brightness));
		colors_buffer.push_back(glm::vec3(brightness));
		colors_buffer.push_back(glm::vec3(brightness));
		colors_buffer.push_back(glm::vec3(brightness));
		colors_buffer.push_back(glm::vec3(brightness));
		colors_buffer.push_back(glm::vec3(brightness));
		colors_buffer.push_back(glm::vec3(brightness));
	}
	glGenBuffers(1, &m_color_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_color_buffer);
	glBufferData(GL_ARRAY_BUFFER,
		colors_buffer.size() * sizeof(glm::vec3),
		colors_buffer.data(), GL_STATIC_DRAW
	);
	glVertexAttribPointer(
		color_attrib_index, 3, GL_FLOAT, false, sizeof(glm::vec3), static_cast<GLvoid*>(0)
	);
	glEnableVertexAttribArray(color_attrib_index);

	// unbind buffers to avoid accidental modification
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

OpenGLGridRenderer::~OpenGLGridRenderer() {
	glDeleteBuffers(1, &m_positions_buffer);
	glDeleteBuffers(1, &m_color_buffer);

	glDeleteVertexArrays(1, &m_vertex_array);
}

void OpenGLGridRenderer::render() {
	assert(m_vertex_array != 0);

	glDisable(GL_FRAMEBUFFER_SRGB);

	glBindVertexArray(m_vertex_array);
	glLineWidth(1.0f);
	glDrawArrays(GL_LINES, 0, 8 * m_distance);

	// unbind to avoid accidental modification
	glBindVertexArray(0);
}
