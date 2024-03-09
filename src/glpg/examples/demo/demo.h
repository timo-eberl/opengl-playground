#pragma once


#include <glad/glad.h>
#include <GLFW/glfw3.h> // include glfw after glad

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
	Scene scene;
	OpenGLRenderer renderer;
};

} // glpg::demo
