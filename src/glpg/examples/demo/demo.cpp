#include <iostream>
#include <chrono>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "glpg/gltf.h"

#include "demo.h"

using namespace glpg::demo;

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	auto state = static_cast<State *>(glfwGetWindowUserPointer(window));

	glViewport(0, 0, width, height);

	state->camera->set_aspect_ratio(static_cast<float>(width) / static_cast<float>(height));
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	auto state = static_cast<State *>(glfwGetWindowUserPointer(window));

	if (state) {
		if (yoffset == 1.0) {
			state->camera_controls->scroll_callback(glpg::CameraViewportControls::UP);
		}
		else if (yoffset == -1.0) {
			state->camera_controls->scroll_callback(glpg::CameraViewportControls::DOWN);
		}
	}
}

static void create_scene(State& state) {
	state.scene = {};

	// copy the default material and modify it
	const auto tex_mat = std::make_shared<glpg::Material>(*state.scene.default_material);
	tex_mat->uniforms["albedo_tex"] = glpg::make_uniform(
		std::make_shared<glpg::Texture>("textures/container.jpg", glpg::Texture::Format::RGB)
	);

	state.scene.add(glpg::gltf::import("models/antique_camera/antique_camera.glb"));
	const auto cube = glpg::gltf::import("models/cube/cube.gltf").get_mesh_nodes()[0];
	cube->get_mesh()->sections[0].material = tex_mat;
	state.scene.add(cube);

	state.renderer.preload(state.scene);
}

static void initialize(GLFWwindow* window, State& state) {
	state.renderer.set_clear_color(glm::vec4(0.231f, 0.231f, 0.231f, 1.0f));

	create_scene(state);

	state.camera = std::make_shared<glpg::PerspectiveCamera>(40.0f, 1300.0f/900.0f, 0.1f, 1000.0f);

	const auto initial_camera_rotation = glm::vec2(glm::radians(-24.2f), glm::radians(63.6f));
	state.camera_controls = std::make_unique<glpg::CameraViewportControls>(initial_camera_rotation);
	state.camera_controls->set_target(glm::vec3(0.0f, 3.0f, 0.0f));

	glfwSetScrollCallback(window, scroll_callback);
}

static void process(GLFWwindow* window, State& state) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	state.camera_controls->update(*window, *state.camera);

	if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS) {
		state.scene.reload_all_shaders();
		state.scene.reload_all_textures();
	}
}

static void render(GLFWwindow* window, State& state) {
	state.renderer.render(state.scene, *state.camera);
}

int glpg::demo::run() {
	glfwInit();

	// tell GLFW we are using OpenGL 4.6
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	// tell GLFW we want to use the core-profile -> no backwards-compatible features
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(1300, 900, "OpenGL Playground", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	// glfwSwapInterval(0); // request to disable vsync

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	{ // this scope ensures that "state" is destroyed before the opengl context is destroyed (glfwTerminate)
		State state {};
		glfwSetWindowUserPointer(window, static_cast<void *>(&state));
		initialize(window, state);

		auto last_frame_time_point = std::chrono::high_resolution_clock::now();

		while (!glfwWindowShouldClose(window)) {
			process(window, state);

			render(window, state);

			// profiling
			auto now = std::chrono::high_resolution_clock::now();
			auto delta = now - last_frame_time_point;
			last_frame_time_point = now;
			auto fps = 1.0 / (delta.count() * 1.0E-9);
			// don't forget to disable vsync!
			// std::cout << "fps: " << fps << ", delta: " << delta << std::endl;

			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}

	glfwTerminate();
	return 0;
}
