#pragma once

#include <glad/glad.h>
// include glfw after glad
#include <GLFW/glfw3.h>

namespace glpg::template_app {

int run();

struct State {};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process(GLFWwindow* window, State& state);
void render(GLFWwindow* window, State& state);
void initialize(GLFWwindow* window, State& state);

} // glpg::template_app
