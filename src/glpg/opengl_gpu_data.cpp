#include "opengl_rendering.h"

#include "log.h"

using namespace glpg;

OpenGLSceneGPUData::OpenGLSceneGPUData(const std::weak_ptr<Scene> scene) : m_scene(scene) {
	assert(!m_scene.expired());
	auto sp_scene = m_scene.lock();
	sp_scene->add_observer(this);

	for (const auto &mesh_node : sp_scene->get_mesh_nodes()) {
		on_mesh_node_added(mesh_node);
	}

	// the error shader must always be present
	const auto error_gpu_data = setup_shader_program(*sp_scene->m_error_shader_program);
	assert(error_gpu_data.creation_successful);
	m_shader_programs.emplace(sp_scene->m_error_shader_program, error_gpu_data);

	// create all preload shader programs
	for (const auto &shader_program : sp_scene->preload_shader_programs) {
		if (m_shader_programs.contains(shader_program)) continue;

		const auto gpu_data = setup_shader_program(*shader_program);
		m_shader_programs.emplace(
			shader_program, gpu_data.creation_successful ? gpu_data : error_gpu_data
		);
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

const OpenGLGeometryGPUData & OpenGLSceneGPUData::get_geometry_gpu_data(
	const std::shared_ptr<Geometry> geometry
) {
	if (!m_geometries.contains(geometry)) {
		log::warn(
			"get_geometry_gpu_data did not find the requested geometry. "
			"Geometry will be added to the scene gpu data.");
		OpenGLGeometryGPUData gpu_data = setup_geometry(*geometry);
		m_geometries.emplace(geometry, gpu_data);
	}
	return m_geometries[geometry];
}

const OpenGLShaderProgramGPUData & OpenGLSceneGPUData::get_shader_program_gpu_data(
	const std::shared_ptr<ShaderProgram> shader_program
) {
	// create shader program if it does not exist yet
	if (!m_shader_programs.contains(shader_program)) {
		log::warn(
			"get_shader_program_gpu_data did not find the requested ShaderProgram. "
			"ShaderProgram will be added to the scene gpu data.");
		auto gpu_data = setup_shader_program(*shader_program);
		if (!gpu_data.creation_successful) {
			log::error("Shader creation failed. Replacing with error shader.", false);

			assert(m_shader_programs.contains(m_scene.lock()->m_error_shader_program));
			gpu_data = m_shader_programs[m_scene.lock()->m_error_shader_program];
		}
		m_shader_programs.emplace(shader_program, gpu_data);
	}
	return m_shader_programs[shader_program];
}

void OpenGLSceneGPUData::on_mesh_node_added(const std::shared_ptr<MeshNode> mesh_node) {
	for (const auto &mesh_section : mesh_node->get_mesh()->sections) {
		// create geometry if it does not exist yet
		if (!m_geometries.contains(mesh_section.geometry)) {
			OpenGLGeometryGPUData gpu_data = setup_geometry(*mesh_section.geometry);
			m_geometries.emplace(mesh_section.geometry, gpu_data);
		}
		// check if a ShaderProgram is assigned
		if (mesh_section.material && mesh_section.material->shader_program) {
			// create shader program if it does not exist yet
			if (!m_shader_programs.contains(mesh_section.material->shader_program)) {
				auto gpu_data = setup_shader_program(*mesh_section.material->shader_program);
				if (!gpu_data.creation_successful) {
					log::error("Shader creation failed. Replacing with error shader.", false);

					assert(m_shader_programs.contains(m_scene.lock()->m_error_shader_program));
					gpu_data = m_shader_programs[m_scene.lock()->m_error_shader_program];
				}
				m_shader_programs.emplace(mesh_section.material->shader_program, gpu_data);
			}
		}
	}
}

void OpenGLSceneGPUData::on_mesh_node_removed(const std::shared_ptr<MeshNode> mesh_node) {
	// release all resources that are unused after the MeshNode was removed
	// geometries
	for (const auto &to_be_removed_mesh_section : mesh_node->get_mesh()->sections) {
		if (is_last_usage(to_be_removed_mesh_section.geometry, mesh_node)) {
			release_geometry(m_geometries[to_be_removed_mesh_section.geometry]);
			m_geometries.erase(to_be_removed_mesh_section.geometry);
		}
	}
	// shader programs
	for (const auto &to_be_removed_mesh_section : mesh_node->get_mesh()->sections) {
		if (!to_be_removed_mesh_section.material || !to_be_removed_mesh_section.material->shader_program) {
			continue; // do nothing if material or shader program is not set
		}
		if (is_last_usage(to_be_removed_mesh_section.material->shader_program, mesh_node)) {
			release_shader_program(m_shader_programs[to_be_removed_mesh_section.material->shader_program]);
			m_shader_programs.erase(to_be_removed_mesh_section.material->shader_program);
		}
	}
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

	gpu_data = {};
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

OpenGLShaderProgramGPUData OpenGLSceneGPUData::setup_shader_program(
	const ShaderProgram &shader_program
) const {
	const uint message_size = 1024;
	GLchar message[message_size];

	GLint vertex_compilation_success = false;
	auto vertex_shader = compile_shader(
		shader_program.vertex_source.c_str(), VERTEX, &vertex_compilation_success, message, message_size
	);
	if (!vertex_compilation_success) {
		log::error(
			std::string("Vertex shader compilation failed (")
			+ shader_program.vertex_shader_path + "):" , false
		);
		log::error(message);
	}

	GLint fragment_compilation_success = false;
	auto fragment_shader = compile_shader(
		shader_program.fragment_source.c_str(), FRAGMENT, &fragment_compilation_success, message, message_size
	);
	if (!fragment_compilation_success) {
		log::error(
			std::string("Fragment shader compilation failed (")
			+ shader_program.fragment_shader_path + "):" , false
		);
		log::error(message);
	}

	// check if compilation failed
	if (!vertex_compilation_success || !fragment_compilation_success) {
		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);
		return {};
	}

	GLuint program_id = glCreateProgram();
	glAttachShader(program_id, vertex_shader);
	glAttachShader(program_id, fragment_shader);
	glLinkProgram(program_id);
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	GLint linkage_success;
	glGetProgramiv(program_id, GL_LINK_STATUS, &linkage_success);

	if (!linkage_success) {
		const uint message_size = 1024;
		GLchar message[message_size];
		glGetProgramInfoLog(program_id, message_size, NULL, message);

		log::error(
			std::string("Linking shader program failed (")
			+ shader_program.vertex_shader_path + ", "
			+ shader_program.fragment_shader_path + "):" , false
		);
		log::error(std::string(message));

		glDeleteProgram(program_id);
		return {};
	}

	return { program_id, true };
}

GLuint OpenGLSceneGPUData::compile_shader(
	const std::string & source, ShaderType shader_type,
	GLint *was_successful, GLchar *message, const uint message_size
) const {
	GLuint shader = 0;
	switch (shader_type) {
		case VERTEX: { shader = glCreateShader(GL_VERTEX_SHADER); } break;
		case FRAGMENT: { shader = glCreateShader(GL_FRAGMENT_SHADER); } break;
	}

	const GLchar* source_c_str = source.c_str();
	glShaderSource(shader, 1, &source_c_str, NULL);
	glCompileShader(shader);

	glGetShaderiv(shader, GL_COMPILE_STATUS, was_successful);
	if (!was_successful) {
		glGetShaderInfoLog(shader, message_size, NULL, message);
	}

	return shader;
}

void OpenGLSceneGPUData::release_shader_program(OpenGLShaderProgramGPUData & gpu_data) const {
	glDeleteProgram(gpu_data.id);
	gpu_data = {};
}

bool OpenGLSceneGPUData::is_last_usage(
	const std::shared_ptr<ShaderProgram> to_be_removed_shader_program,
	const std::shared_ptr<MeshNode> to_be_removed_node
) {
	for (const auto &staying_node : m_scene.lock()->get_mesh_nodes()) {
		if (staying_node == to_be_removed_node) { continue; }

		for (const auto &staying_mesh_section : staying_node->get_mesh()->sections) {
			if (staying_mesh_section.material->shader_program == to_be_removed_shader_program) {
				return false;
			}
		}
	}
	return true;
}
