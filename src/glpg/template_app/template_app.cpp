#include <iostream>
#include <chrono>

#include "template_app.h"

using namespace glpg;

int template_app::run() {
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

	State state {};
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

	glfwTerminate();
	return 0;
}

void template_app::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void template_app::initialize(GLFWwindow* window, State& state) { }

void template_app::process(GLFWwindow* window, State& state) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

void template_app::render(GLFWwindow* window, State& state) {
	glClearColor(0.32f, 0.27f, 0.40f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}
