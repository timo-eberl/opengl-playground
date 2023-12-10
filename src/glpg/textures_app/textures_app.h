#pragma once

#include <glad/glad.h>
// include glfw after glad
#include <GLFW/glfw3.h>

#include <memory>

#include "glpg/shader_program.h"
#include "glpg/uniforms.h"

namespace glpg::textures_app {

struct State {
	Uniforms uniforms;
	std::unique_ptr<ShaderProgram> shader_program;
	GLuint vertex_array = 0;
	GLuint texture_0_id = 0;
	GLuint texture_1_id = 0;
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process(GLFWwindow* window, State& state);
void render(GLFWwindow* window, State& state);
void initialize(GLFWwindow* window, State& state);
int run();

} // glpg::textures_app
