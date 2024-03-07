#pragma once

#include <glad/glad.h>
// include glfw after glad
#include <GLFW/glfw3.h>

namespace glpg::hello_triangle {

struct State {
	GLuint program_0 = 0;
	GLuint vertex_array_0 = 0;
	GLuint program_1 = 0;
	GLuint vertex_array_1 = 0;
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process(GLFWwindow* window, State& state);
void render(GLFWwindow* window, State& state);
void initialize(GLFWwindow* window, State& state);
int run();

} // glpg::hello_triangle
