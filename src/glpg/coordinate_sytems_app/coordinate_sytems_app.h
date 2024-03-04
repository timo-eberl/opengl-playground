#pragma once

#include <glad/glad.h>
// include glfw after glad
#include <GLFW/glfw3.h>

#include "glpg/shader_program.h"
#include "glpg/uniforms.h"
#include "glpg/perspective_camera.h"
#include "glpg/camera_orbit_controls.h"
#include "glpg/meshes/mesh.h"

namespace glpg::coordinate_sytems_app {

int run();

struct State {
	Uniforms uniforms;
	std::unique_ptr<ShaderProgram> shader_program;
	GLuint vertex_array = 0;
	std::shared_ptr<PerspectiveCamera> camera;
	std::unique_ptr<CameraOrbitControls> camera_controls;
	// int index_count = 0;
	// bool index_format_short = false;
	meshes::Mesh mesh;
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process(GLFWwindow* window, State& state);
void render(GLFWwindow* window, State& state);
void initialize(GLFWwindow* window, State& state);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

} // glpg::coordinate_sytems_app
