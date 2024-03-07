#pragma once

// include glfw after glad
#include <GLFW/glfw3.h>

#include "glpg/shader_program.h"
#include "glpg/uniforms.h"
#include "glpg/perspective_camera.h"
#include "glpg/camera_viewport_controls.h"
#include "glpg/scene.h"
#include "glpg/opengl_rendering.h"

namespace glpg::demo {

int run();

struct State {
	std::shared_ptr<PerspectiveCamera> camera;
	std::unique_ptr<CameraViewportControls> camera_controls;
	std::shared_ptr<Scene> scene;
	OpenGLRenderer renderer;
	std::shared_ptr<OpenGLSceneGPUData> scene_gpu_data;
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process(GLFWwindow* window, State& state);
void render(GLFWwindow* window, State& state);
void initialize(GLFWwindow* window, State& state);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

} // glpg::demo
