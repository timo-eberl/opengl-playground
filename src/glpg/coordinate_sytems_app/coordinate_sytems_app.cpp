#include <iostream>
#include <chrono>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "glpg/shader_program.h"
#include "glpg/gltf_importer.h"

#include "coordinate_sytems_app.h"

using namespace glpg;

int coordinate_sytems_app::run() {
	glfwInit();

	// tell GLFW we are using OpenGL 4.6
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	// tell GLFW we want to use the core-profile -> no backwards-compatible features
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Playground", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	// glfwSwapInterval(0); // request to disable vsync

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	{ // this scope ensures that "state" is destroyed before the opengl context is destroyed (glfwTerminate)
		State state {};
		glfwSetWindowUserPointer(window, static_cast<void *>(&state));
		initialize(window, state);

		auto last_frame_time_point = std::chrono::high_resolution_clock::now();

		while (!glfwWindowShouldClose(window)) {
			process(window, state);

			render(window, state);

			// profiling
			auto now = std::chrono::high_resolution_clock::now();
			auto delta = now - last_frame_time_point;
			last_frame_time_point = now;
			auto fps = 1.0 / (delta.count() * 1.0E-9);
			// don't forget to disable vsync!
			// std::cout << "fps: " << fps << ", delta: " << delta << std::endl;

			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}

	glfwTerminate();
	return 0;
}

void coordinate_sytems_app::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void coordinate_sytems_app::initialize(GLFWwindow* window, State& state) {
	state.shader_program = std::make_unique<ShaderProgram>(
		"shaders/coordinate_sytems_app/shader.vert", "shaders/coordinate_sytems_app/shader.frag");

	const auto antique_camera = gltf_importer::import("models/antique_camera/antique_camera.glb");
	const auto cube = gltf_importer::import("models/cube/cube.gltf");
	state.mesh_nodes.insert(state.mesh_nodes.end(), antique_camera.begin(), antique_camera.end());
	state.mesh_nodes.insert(state.mesh_nodes.end(), cube.begin(), cube.end());
	for (const auto & mesh_node : state.mesh_nodes) {
		for (auto & section : mesh_node.get_mesh()->sections) {
			section.send_to_gpu();
		}
	}

	// cube with position and uvs
	static const GLfloat vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};
	static const GLuint indices[] = {
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
		27, 28, 29, 30, 31, 32, 33, 34, 35
	};

	glEnable(GL_DEPTH_TEST);

	state.camera = std::make_shared<PerspectiveCamera>(55.0f, 800.0f/600.0f, 0.1f, 1000.0f);

	const auto initial_rotation = glm::vec2(glm::radians(-24.2f), glm::radians(63.6f));
	state.camera_controls = std::make_unique<CameraOrbitControls>(state.camera, initial_rotation);

	glfwSetScrollCallback(window, scroll_callback);
}

void coordinate_sytems_app::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	auto state = static_cast<State *>(glfwGetWindowUserPointer(window));

	if (state) {
		if (yoffset == 1.0) {
			state->camera_controls->scroll_callback(CameraOrbitControls::UP);
		}
		else if (yoffset == -1.0) {
			state->camera_controls->scroll_callback(CameraOrbitControls::DOWN);
		}
	}
}

void coordinate_sytems_app::process(GLFWwindow* window, State& state) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	state.camera_controls->update(*window);

	if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS) {
		state.shader_program->recreate();
	}
}

void coordinate_sytems_app::render(GLFWwindow* window, State& state) {
	glClearColor(0.32f, 0.27f, 0.40f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	state.uniforms.set_mat4("view_matrix", glm::inverse(state.camera->get_model_matrix()));
	state.uniforms.set_mat4("projection_matrix", state.camera->get_projection_matrix());

	state.shader_program->use();
	for (const auto &mesh_node : state.mesh_nodes) {
		state.uniforms.set_mat4("model_matrix", mesh_node.get_model_matrix());

		state.uniforms.apply_to_program(*state.shader_program);
		mesh_node.get_mesh()->draw();
	}

	// unbind to avoid accidental modification
	glBindVertexArray(0);
}
