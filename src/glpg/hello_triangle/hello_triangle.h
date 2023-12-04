#pragma once

#include <glad/glad.h>
// include glfw after glad
#include <GLFW/glfw3.h>

namespace glpg::hello_triangle {

struct State {
	GLuint program = 0;
	GLuint vertex_array = 0;
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process(GLFWwindow* window, State& state);
void render(GLFWwindow* window, State& state);
void initialize(GLFWwindow* window, State& state);
int run();

} // glpg::hello_triangle
