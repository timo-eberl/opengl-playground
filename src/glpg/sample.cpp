#include <glad/glad.h>
// include glfw after glad
#include <GLFW/glfw3.h>

#include <iostream>

#include "sample.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process(GLFWwindow* window);
void render(GLFWwindow* window);

int glpg::empty_window() {
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

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	while (!glfwWindowShouldClose(window)) {
		process(window);

		render(window);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void process(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

void render(GLFWwindow* window) {
	glClearColor(0.32f, 0.27f, 0.40f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}
