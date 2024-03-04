#include "geometry.h"

#include <iostream>

using glpg::meshes::Geometry;
using glpg::meshes::GeometryData;

Geometry::Geometry(const GeometryData geometry_data) : m_geometry_data(std::move(geometry_data)) { setup(); }

void Geometry::setup() {
	static const GLint position_attrib_index = 0; // layout (location = 0)
	static const GLint normal_attrib_index = 1; // layout (location = 1)
	static const GLint uv_attrib_index = 2; // layout (location = 2)

	// any subsequent vertex attribute calls and bind buffer calls
	// will be stored inside the vertex array
	glGenVertexArrays(1, &m_vertex_array);
	glBindVertexArray(m_vertex_array);

	glGenBuffers(1, &m_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER,
		m_geometry_data.vertices.size() * sizeof(GLfloat),
		m_geometry_data.vertices.data(),
	GL_STATIC_DRAW);

	glGenBuffers(1, &m_index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		m_geometry_data.indices.size() * sizeof(GLuint),
		m_geometry_data.indices.data(),
	GL_STATIC_DRAW);

	GLsizei vertex_stride = m_geometry_data.vertices_num_components * sizeof(GLfloat);

	glVertexAttribPointer(
		position_attrib_index, m_geometry_data.vertices_position_num_components, GL_FLOAT, false,
		vertex_stride, reinterpret_cast<GLvoid*>(m_geometry_data.vertices_position_offset * sizeof(GLfloat))
	);
	glEnableVertexAttribArray(position_attrib_index);

	glVertexAttribPointer(
		normal_attrib_index, m_geometry_data.vertices_normal_num_components, GL_FLOAT, false,
		vertex_stride, reinterpret_cast<GLvoid*>(m_geometry_data.vertices_normal_offset * sizeof(GLfloat))
	);
	glEnableVertexAttribArray(normal_attrib_index);

	glVertexAttribPointer(
		uv_attrib_index, m_geometry_data.vertices_uv_num_components, GL_FLOAT, false,
		vertex_stride, reinterpret_cast<GLvoid*>(m_geometry_data.vertices_uv_offset * sizeof(GLfloat))
	);
	glEnableVertexAttribArray(uv_attrib_index);

	// unbind buffers to avoid accidental modification
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Geometry::~Geometry() { release(); }

void Geometry::release() {
	glDeleteBuffers(1, &m_vertex_buffer);
	glDeleteBuffers(1, &m_index_buffer);

	glDeleteVertexArrays(1, &m_vertex_array);
}

void Geometry::draw() const {
	glBindVertexArray(m_vertex_array);
	glDrawElements(GL_TRIANGLES, m_geometry_data.indices.size(), GL_UNSIGNED_INT, NULL);
	// unbind to avoid accidental modification
	glBindVertexArray(0);
}
