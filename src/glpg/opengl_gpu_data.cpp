#include "opengl_rendering.h"

using namespace glpg;

OpenGLSceneGPUData::OpenGLSceneGPUData(const std::weak_ptr<Scene> scene) : m_scene(scene) {
	assert(!m_scene.expired());
	auto sp_scene = m_scene.lock();
	sp_scene->add_observer(this);

	for (const auto & mesh_node : sp_scene->get_mesh_nodes()) {
		on_mesh_node_added(mesh_node);
	}
}

OpenGLSceneGPUData::~OpenGLSceneGPUData() {
	if (auto sp_scene = m_scene.lock()) {
		sp_scene->remove_observer(this);
	}
}

bool OpenGLSceneGPUData::belongs_to(const Scene &scene) const {
	return !m_scene.expired() && &scene == m_scene.lock().get();
}

const OpenGLGeometryGPUData & OpenGLSceneGPUData::get_geometry_gpu_data(const std::shared_ptr<Geometry> geometry) {
	assert(m_geometries.contains(geometry));
	return m_geometries[geometry];
}

void OpenGLSceneGPUData::on_mesh_node_added(const std::shared_ptr<MeshNode> mesh_node) {
	for (const auto &mesh_section : mesh_node->get_mesh()->sections) {
		if (m_geometries.contains(mesh_section.geometry)) continue;

		OpenGLGeometryGPUData gpu_data = setup_geometry(*mesh_section.geometry);
		m_geometries.emplace(mesh_section.geometry, gpu_data);
	}
}

void OpenGLSceneGPUData::on_mesh_node_removed(const std::shared_ptr<MeshNode> mesh_node) {
	// release (gpu) all now unused geometries
	for (const auto &to_be_removed_mesh_section : mesh_node->get_mesh()->sections) {
		if (is_last_usage(to_be_removed_mesh_section.geometry, mesh_node)) {
			release_geometry(m_geometries[to_be_removed_mesh_section.geometry]);
			m_geometries.erase(to_be_removed_mesh_section.geometry);
		}
	}
}

bool OpenGLSceneGPUData::is_last_usage(
	const std::shared_ptr<Geometry> to_be_removed_geometry,
	const std::shared_ptr<MeshNode> to_be_removed_node
) {
	for (const auto &staying_node : m_scene.lock()->get_mesh_nodes()) {
		if (staying_node == to_be_removed_node) { continue; }

		for (const auto &staying_mesh_section : staying_node->get_mesh()->sections) {
			if (staying_mesh_section.geometry == to_be_removed_geometry) {
				return false;
			}
		}
	}
	return true;
}

OpenGLGeometryGPUData OpenGLSceneGPUData::setup_geometry(const Geometry &geometry) const {
	OpenGLGeometryGPUData gpu_data = {};

	static const GLint position_attrib_index = 0; // layout (location = 0)
	static const GLint normal_attrib_index = 1; // layout (location = 1)
	static const GLint uv_attrib_index = 2; // layout (location = 2)

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

	// unbind buffers to avoid accidental modification
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return gpu_data;
}

void OpenGLSceneGPUData::release_geometry(OpenGLGeometryGPUData & gpu_data) const {
	glDeleteBuffers(1, &gpu_data.index_buffer);
	glDeleteBuffers(1, &gpu_data.positions_buffer);
	glDeleteBuffers(1, &gpu_data.normals_buffer);
	glDeleteBuffers(1, &gpu_data.uvs_buffer);

	glDeleteVertexArrays(1, &gpu_data.vertex_array);
}
