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

	state.mesh = gltf_importer::import("models/antique_camera/antique_camera.glb");

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

	state.camera = std::make_shared<PerspectiveCamera>(45.0f, 800.0f/600.0f, 0.1f, 100.0f);
	glm::mat4 cam_model_matrix = glm::translate(glm::identity<glm::mat4>(), glm::vec3(0.0f, 5.0f, 7.0f));
	cam_model_matrix = glm::rotate(cam_model_matrix, glm::radians(-25.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	state.camera->set_model_matrix(cam_model_matrix);

	state.camera_controls = std::make_unique<CameraOrbitControls>(state.camera);

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

	static const glm::vec3 cube_positions[] = {
		glm::vec3( 0.0f,  0.0f,  0.0f),
		glm::vec3( 2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3( 2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3( 1.3f, -2.0f, -2.5f),
		glm::vec3( 1.5f,  2.0f, -2.5f),
		glm::vec3( 1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	state.uniforms.set_mat4("view_matrix", glm::inverse(state.camera->get_model_matrix()));
	state.uniforms.set_mat4("projection_matrix", state.camera->get_projection_matrix());

	state.shader_program->use();
	for (size_t i = 0; i < 1; i++) {
		glm::mat4 model = glm::identity<glm::mat4>();
		model = glm::translate(model, cube_positions[i]);
		// model = glm::rotate(
		// 	model, (float)glfwGetTime() * glm::radians(50.0f * (i%3+1)), glm::vec3(0.5f, (i+3)/10.0f, 0.0f)
		// );
		// model = glm::scale(model, glm::vec3(0.5, 1.0, 1.5));
		state.uniforms.set_mat4("model_matrix", model);
	
		state.uniforms.apply_to_program(*state.shader_program);
		state.mesh.draw();
	}

	// unbind to avoid accidental modification
	glBindVertexArray(0);
}
