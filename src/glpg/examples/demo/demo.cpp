#include <iostream>
#include <chrono>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "glpg/shader_program.h"
#include "glpg/gltf.h"

#include "demo.h"

using namespace glpg;

int demo::run() {
	glfwInit();

	// tell GLFW we are using OpenGL 4.6
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	// tell GLFW we want to use the core-profile -> no backwards-compatible features
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(1300, 900, "OpenGL Playground", NULL, NULL);
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

void demo::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	auto state = static_cast<State *>(glfwGetWindowUserPointer(window));

	glViewport(0, 0, width, height);

	state->camera->set_aspect_ratio(static_cast<float>(width) / static_cast<float>(height));
}

void demo::initialize(GLFWwindow* window, State& state) {
	state.renderer.set_clear_color(glm::vec4(0.231f, 0.231f, 0.231f, 1.0f));

	state.scene = std::make_shared<Scene>();
	state.scene->add(gltf::import("models/antique_camera/antique_camera.glb"));
	state.scene->add(gltf::import("models/cube/cube.gltf"));
	state.scene_gpu_data = std::make_shared<OpenGLSceneGPUData>(state.scene);

	state.camera = std::make_shared<PerspectiveCamera>(40.0f, 1300.0f/900.0f, 0.1f, 1000.0f);

	const auto initial_camera_rotation = glm::vec2(glm::radians(-24.2f), glm::radians(63.6f));
	state.camera_controls = std::make_unique<CameraViewportControls>(initial_camera_rotation);

	glfwSetScrollCallback(window, scroll_callback);
}

void demo::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	auto state = static_cast<State *>(glfwGetWindowUserPointer(window));

	if (state) {
		if (yoffset == 1.0) {
			state->camera_controls->scroll_callback(CameraViewportControls::UP);
		}
		else if (yoffset == -1.0) {
			state->camera_controls->scroll_callback(CameraViewportControls::DOWN);
		}
	}
}

void demo::process(GLFWwindow* window, State& state) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	state.camera_controls->update(*window, *state.camera);

	if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS) {
		state.scene = std::make_shared<Scene>();
		state.scene->add(gltf::import("models/antique_camera/antique_camera.glb"));
		state.scene->add(gltf::import("models/cube/cube.gltf"));
	}
}

void demo::render(GLFWwindow* window, State& state) {
	assert(state.scene_gpu_data && state.scene); // make sure scene and scene data are not null
	state.renderer.render(*state.scene, *state.camera, *state.scene_gpu_data);
}