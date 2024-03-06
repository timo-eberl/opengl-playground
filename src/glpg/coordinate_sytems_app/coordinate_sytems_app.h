#pragma once

#include <glad/glad.h>
// include glfw after glad
#include <GLFW/glfw3.h>

#include "glpg/shader_program.h"
#include "glpg/uniforms.h"
#include "glpg/perspective_camera.h"
#include "glpg/camera_viewport_controls.h"
#include "glpg/scene.h"
#include "glpg/renderer.h"

namespace glpg::coordinate_sytems_app {

int run();

struct State {
	std::shared_ptr<PerspectiveCamera> camera;
	std::unique_ptr<CameraViewportControls> camera_controls;
	Scene scene;
	Renderer renderer;
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process(GLFWwindow* window, State& state);
void render(GLFWwindow* window, State& state);
void initialize(GLFWwindow* window, State& state);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

} // glpg::coordinate_sytems_app
