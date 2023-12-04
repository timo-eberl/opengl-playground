#pragma once

#include <glad/glad.h>
// include glfw after glad
#include <GLFW/glfw3.h>

namespace glpg::template_app {

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process(GLFWwindow* window);
void render(GLFWwindow* window);
int run();

} // glpg::template_app
