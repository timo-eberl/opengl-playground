#include <glad/glad.h>
#include <GLFW/glfw3.h> // include glfw after glad

#include <iostream>
#include <chrono>
#include <sstream>

#include <glm/glm.hpp>

#include <ron.h>

using namespace ron;

static const auto initial_resolution = glm::uvec2(1300, 900);

struct State {
	std::shared_ptr<PerspectiveCamera> camera;
	std::unique_ptr<CameraViewportControls> camera_controls;
	Scene scene;
	std::unique_ptr<OpenGLRenderer> renderer;
};

static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
static void create_scene(State& state);
static void initialize(GLFWwindow* window, State& state);
static void process(GLFWwindow* window, State& state);
static void render(GLFWwindow* window, State& state);

int main() {
	glfwInit();

	// tell GLFW we are using OpenGL 4.6
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	// tell GLFW we want to use the core-profile -> no backwards-compatible features
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(
		initial_resolution.x, initial_resolution.y, "Ron", NULL, NULL
	);
	if (window == NULL) {
		log::error("Failed to create GLFW window");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(0); // request to disable vsync

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
		auto frame_counter = 0;
		auto last_fps_update = std::chrono::high_resolution_clock::now();

		while (!glfwWindowShouldClose(window)) {
			process(window, state);
			render(window, state);

			// profiling
			using namespace std::chrono_literals;
			const auto now = std::chrono::high_resolution_clock::now();
			const auto delta = now - last_frame_time_point;
			last_frame_time_point = now;

			// FPS in window title - pretty inaccurate measurement
			frame_counter++;
			const auto time_since_last_fps_update = now - last_fps_update;
			static const auto fps_updates_per_second = 5.0; // if set to high, application crashes
			if (time_since_last_fps_update > 1s / fps_updates_per_second) {
				const auto fps = frame_counter * fps_updates_per_second;
				std::stringstream title_stream;
				title_stream << "Ron | FPS: " << fps;
				glfwSetWindowTitle(window,  + title_stream.str().c_str());

				frame_counter = 0;
				last_fps_update = now;
			}

			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}

	glfwTerminate();
	return 0;
}

void create_scene(State& state) {
	state.scene = {};

	// copy the default material and modify it
	const auto tex_mat = std::make_shared<ron::Material>(*state.scene.default_material);
	tex_mat->uniforms["albedo_tex"] = ron::make_uniform(
		assets::load_texture("textures/awesomeface.png")
	);

	state.scene.add(ron::gltf::import("models/antique_camera/antique_camera.glb"));
	state.scene.add(ron::gltf::import("models/shadow_test_scene/shadow_test_scene.glb"));
	// state.scene.add(ron::gltf::import("models/gravel_torus/gravel_torus.glb"));
	// state.scene.add(ron::gltf::import("models/gravel_torus/gravel_torus_without_tangents.glb"));
	// state.scene.add(ron::gltf::import("default/models/cube/cube.gltf"));

	// const auto cube = ron::gltf::import("default/models/cube/cube.gltf").get_mesh_nodes()[0];
	// cube->get_mesh()->sections[0].material = tex_mat;
	// state.scene.add(cube);

	DirectionalLight directional_light = {};
	directional_light.world_position = glm::vec3(4.1f, 5.9f, -1.0f);
	directional_light.world_direction = glm::normalize(glm::vec3(4.1f, 5.9f, -1.0f));
	directional_light.shadow.enabled = true;
	directional_light.shadow.map_size = glm::uvec2(512);
	directional_light.shadow.bias = 0.02f;
	directional_light.shadow.far = 50.0f;
	directional_light.shadow.frustum_size = 6.0f;
	state.scene.set_directional_light(directional_light);

	state.renderer->preload(state.scene);
}

void initialize(GLFWwindow* window, State& state) {
	state.renderer = std::make_unique<ron::OpenGLRenderer>(initial_resolution);

	state.renderer->set_clear_color(glm::vec4(0.231f, 0.231f, 0.231f, 1.0f));

	create_scene(state);

	const auto aspect_ratio
		= static_cast<float>(initial_resolution.x) / static_cast<float>(initial_resolution.y);
	state.camera = std::make_shared<ron::PerspectiveCamera>(40.0f, aspect_ratio, 0.1f, 1000.0f);

	const auto initial_camera_rotation = glm::vec2(glm::radians(-24.2f), glm::radians(63.6f));
	state.camera_controls = std::make_unique<ron::CameraViewportControls>(initial_camera_rotation);
	state.camera_controls->set_target(glm::vec3(0.0f, 0.0f, 0.0f));

	glfwSetScrollCallback(window, scroll_callback);
}

void process(GLFWwindow* window, State& state) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
		return;
	}
	state.camera_controls->update(*window, *state.camera);

	if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS) {
		assets::reload_shader_programs();
		assets::reload_textures();
	}
}

void render(GLFWwindow* window, State& state) {
	state.renderer->render(state.scene, *state.camera);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	auto state = static_cast<State *>(glfwGetWindowUserPointer(window));
	assert(state);

	state->renderer->resolution = glm::uvec2(width, height);

	state->camera->set_aspect_ratio(static_cast<float>(width) / static_cast<float>(height));
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	auto state = static_cast<State *>(glfwGetWindowUserPointer(window));

	if (state) {
		if (yoffset == 1.0) {
			state->camera_controls->scroll_callback(ron::CameraViewportControls::UP);
		}
		else if (yoffset == -1.0) {
			state->camera_controls->scroll_callback(ron::CameraViewportControls::DOWN);
		}
	}
}
