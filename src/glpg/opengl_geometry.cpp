#include "opengl_rendering.h"

#include "log.h"

using namespace glpg;

OpenGLGeometryGPUData glpg::opengl_setup_geometry(const Geometry &geometry) {
	OpenGLGeometryGPUData gpu_data = {};

	static const GLint position_attrib_index = 0; // layout (location = 0)
	static const GLint normal_attrib_index = 1; // layout (location = 1)
	static const GLint uv_attrib_index = 2; // layout (location = 2)
	static const GLint tangent_attrib_index = 3; // layout (location = 3)

	// any subsequent vertex attribute calls and bind buffer calls
	// will be stored inside the vertex array
	glGenVertexArrays(1, &gpu_data.vertex_array);
	glBindVertexArray(gpu_data.vertex_array);

	// indices
	glGenBuffers(1, &gpu_data.index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpu_data.index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		geometry.indices.size() * sizeof(GLuint),
		geometry.indices.data(),
	GL_STATIC_DRAW);

	// positions
	glGenBuffers(1, &gpu_data.positions_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, gpu_data.positions_buffer);
	glBufferData(GL_ARRAY_BUFFER,
		geometry.positions.size() * sizeof(glm::vec3),
		geometry.positions.data(),
	GL_STATIC_DRAW);
	GLsizei positions_stride = sizeof(glm::vec3);
	glVertexAttribPointer(position_attrib_index, 3, GL_FLOAT, false, positions_stride, static_cast<GLvoid*>(0));
	glEnableVertexAttribArray(position_attrib_index);

	// normals (optional)
	if (geometry.normals.size() > 0) {
		glGenBuffers(1, &gpu_data.normals_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, gpu_data.normals_buffer);
		glBufferData(GL_ARRAY_BUFFER,
			geometry.normals.size() * sizeof(glm::vec3),
			geometry.normals.data(),
		GL_STATIC_DRAW);
		GLsizei normals_stride = sizeof(glm::vec3);
		glVertexAttribPointer(normal_attrib_index, 3, GL_FLOAT, false, normals_stride, static_cast<GLvoid*>(0));
		glEnableVertexAttribArray(normal_attrib_index);
	}

	// uvs (optional)
	if (geometry.uvs.size() > 0) {
		glGenBuffers(1, &gpu_data.uvs_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, gpu_data.uvs_buffer);
		glBufferData(GL_ARRAY_BUFFER,
			geometry.uvs.size() * sizeof(glm::vec2),
			geometry.uvs.data(),
		GL_STATIC_DRAW);
		GLsizei uvs_stride = sizeof(glm::vec2);
		glVertexAttribPointer(uv_attrib_index, 2, GL_FLOAT, false, uvs_stride, static_cast<GLvoid*>(0));
		glEnableVertexAttribArray(uv_attrib_index);
	}

	// tangents (optional)
	if (geometry.tangents.size() > 0) {
		glGenBuffers(1, &gpu_data.tangents_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, gpu_data.tangents_buffer);
		glBufferData(GL_ARRAY_BUFFER,
			geometry.tangents.size() * sizeof(glm::vec4),
			geometry.tangents.data(),
		GL_STATIC_DRAW);
		GLsizei tangents_stride = sizeof(glm::vec4);
		glVertexAttribPointer(tangent_attrib_index, 4, GL_FLOAT, false, tangents_stride, static_cast<GLvoid*>(0));
		glEnableVertexAttribArray(tangent_attrib_index);
	}

	// unbind buffers to avoid accidental modification
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return gpu_data;
}

void glpg::opengl_release_geometry(OpenGLGeometryGPUData & gpu_data) {
	glDeleteBuffers(1, &gpu_data.index_buffer);
	glDeleteBuffers(1, &gpu_data.positions_buffer);
	glDeleteBuffers(1, &gpu_data.normals_buffer);
	glDeleteBuffers(1, &gpu_data.uvs_buffer);
	glDeleteBuffers(1, &gpu_data.tangents_buffer);

	glDeleteVertexArrays(1, &gpu_data.vertex_array);

	gpu_data = {};
}
