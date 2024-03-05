#include "glpg/meshes.h"

#include <iostream>

using glpg::meshes::RenderGeometry;
using glpg::meshes::GeometryData;

RenderGeometry::RenderGeometry(const GeometryData geometry_data) : m_geometry_data(std::move(geometry_data)) {}

void RenderGeometry::send_to_gpu() {
	if (m_vertex_array != 0) return; // nothing to do

	static const GLint position_attrib_index = 0; // layout (location = 0)
	static const GLint normal_attrib_index = 1; // layout (location = 1)
	static const GLint uv_attrib_index = 2; // layout (location = 2)

	// any subsequent vertex attribute calls and bind buffer calls
	// will be stored inside the vertex array
	glGenVertexArrays(1, &m_vertex_array);
	glBindVertexArray(m_vertex_array);

	glGenBuffers(1, &m_index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		m_geometry_data.indices.size() * sizeof(GLuint),
		m_geometry_data.indices.data(),
	GL_STATIC_DRAW);

	glGenBuffers(1, &m_positions_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_positions_buffer);
	glBufferData(GL_ARRAY_BUFFER,
		m_geometry_data.positions.size() * sizeof(glm::vec3),
		m_geometry_data.positions.data(),
	GL_STATIC_DRAW);

	GLsizei positions_stride = sizeof(glm::vec3);
	glVertexAttribPointer(position_attrib_index, 3, GL_FLOAT, false, positions_stride, static_cast<GLvoid*>(0));
	glEnableVertexAttribArray(position_attrib_index);

	glGenBuffers(1, &m_normals_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_normals_buffer);
	glBufferData(GL_ARRAY_BUFFER,
		m_geometry_data.normals.size() * sizeof(glm::vec3),
		m_geometry_data.normals.data(),
	GL_STATIC_DRAW);

	GLsizei normals_stride = sizeof(glm::vec3);
	glVertexAttribPointer(normal_attrib_index, 3, GL_FLOAT, false, normals_stride, static_cast<GLvoid*>(0));
	glEnableVertexAttribArray(normal_attrib_index);

	glGenBuffers(1, &m_uvs_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_uvs_buffer);
	glBufferData(GL_ARRAY_BUFFER,
		m_geometry_data.uvs.size() * sizeof(glm::vec2),
		m_geometry_data.uvs.data(),
	GL_STATIC_DRAW);

	GLsizei uvs_stride = sizeof(glm::vec2);
	glVertexAttribPointer(uv_attrib_index, 2, GL_FLOAT, false, uvs_stride, static_cast<GLvoid*>(0));
	glEnableVertexAttribArray(uv_attrib_index);

	// unbind buffers to avoid accidental modification
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

RenderGeometry::~RenderGeometry() { release(); }

void RenderGeometry::release() {
	glDeleteBuffers(1, &m_positions_buffer);
	glDeleteBuffers(1, &m_normals_buffer);
	glDeleteBuffers(1, &m_uvs_buffer);
	glDeleteBuffers(1, &m_index_buffer);

	glDeleteVertexArrays(1, &m_vertex_array);
}

void RenderGeometry::draw() const {
	if (m_vertex_array == 0) {
		std::cerr << "\033[1;31m" // font color red
			<< "Cannot draw geometry, because `send_to_gpu` was not called yet.\n\n"
			<< "\033[1;0m"; // reset styling
		return;
	}

	glBindVertexArray(m_vertex_array);
	glDrawElements(GL_TRIANGLES, m_geometry_data.indices.size(), GL_UNSIGNED_INT, NULL);
	// unbind to avoid accidental modification
	glBindVertexArray(0);
}
