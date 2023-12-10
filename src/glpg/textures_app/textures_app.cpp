#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

#include <stb_image.h>

#include "textures_app.h"
#include "glpg/shader_program.h"
#include "glpg/uniforms.h"

#define ASSETS_DIR _ASSETS_DIR

using namespace glpg;

int textures_app::run() {
	glfwInit();

	// tell GLFW we are using OpenGL 4.6
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	// tell GLFW we want to use the core-profile -> no backwards-compatible features
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Playground", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	State state {};
	initialize(window, state);

	while (!glfwWindowShouldClose(window)) {
		process(window, state);

		render(window, state);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void textures_app::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void textures_app::process(GLFWwindow* window, State& state) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS) {
		state.shader_program->recreate();
	}
}

void textures_app::initialize(GLFWwindow* window, State& state) {
	state.shader_program = std::make_unique<ShaderProgram>(
		"shaders/textures_app/shader.vert", "shaders/textures_app/shader.frag");

	state.uniforms.set_float3("min_color", { 0.2f, 0.2f, 0.2f });

	static const GLfloat vertices[] = {
		// positions        	// uv
		 0.0f,   0.5f, 0.0f,	0.5f, 1.0f, // top center
		 0.5f,  -0.5f, 0.0f,	1.0f, 0.0f, // bottom right
		-0.5f,  -0.5f, 0.0f,	0.0f, 0.0f, // bottom left
	};
	static const GLuint indices[] = {
		0, 1, 2,
	};

	static const GLint position_attrib_index = 0; // layout (location = 0)
	static const GLint uv_attrib_index = 1; // layout (location = 1)

	// used to receive error messages later on
	GLint success;
	GLchar message[1024];

	// any subsequent vertex attribute calls and bind buffer calls
	// will be stored inside the vertex array
	glGenVertexArrays(1, &state.vertex_array);
	glBindVertexArray(state.vertex_array);

	GLuint vertex_buffer = 0;
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	GLuint element_buffer = 0;
	glGenBuffers(1, &element_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	GLsizei vertex_stride = 5 * sizeof(GLfloat);
	glVertexAttribPointer(position_attrib_index, 3, GL_FLOAT, false, vertex_stride, (GLvoid*)0);
	glEnableVertexAttribArray(position_attrib_index);
	glVertexAttribPointer(uv_attrib_index, 2, GL_FLOAT, false, vertex_stride, (GLvoid*)(3*sizeof(GLfloat)));
	glEnableVertexAttribArray(uv_attrib_index);

	stbi_set_flip_vertically_on_load(true);

	int tex_width, tex_height, tex_n_channels;
	std::string path = ASSETS_DIR; path += "textures/container.jpg";
	unsigned char* texture_data = stbi_load(path.c_str(), &tex_width, &tex_height, &tex_n_channels, 0);
	if (!texture_data) {
		std::cerr << "\033[1;31m"; // font color red
		std::cerr << "Failed to load texture \"" << path << "\"\n\n";
		std::cerr << "\033[1;0m"; // reset styling
	}
	else {
		glGenTextures(1, &state.texture_0_id);
		glActiveTexture(GL_TEXTURE0); // texture unit
		glBindTexture(GL_TEXTURE_2D, state.texture_0_id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // wrap texture around x
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // wrap texture around y
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_RGB, GL_UNSIGNED_BYTE,
			texture_data
		);
		glGenerateMipmap(GL_TEXTURE_2D);
		state.uniforms.set_int1("tex0", 0);
	}
	stbi_image_free(texture_data);

	path = ASSETS_DIR; path += "textures/awesomeface.png";
	texture_data = stbi_load(path.c_str(), &tex_width, &tex_height, &tex_n_channels, 0);
	if (!texture_data) {
		std::cerr << "\033[1;31m"; // font color red
		std::cerr << "Failed to load texture \"" << path << "\"\n\n";
		std::cerr << "\033[1;0m"; // reset styling
	}
	else {
		glGenTextures(1, &state.texture_1_id);
		glActiveTexture(GL_TEXTURE1); // texture unit
		glBindTexture(GL_TEXTURE_2D, state.texture_1_id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // wrap texture around x
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // wrap texture around y
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
			texture_data
		);
		glGenerateMipmap(GL_TEXTURE_2D);
		state.uniforms.set_int1("tex1", 1);
	}
	stbi_image_free(texture_data);

	// unbind buffers to avoid accidental modification
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void textures_app::render(GLFWwindow* window, State& state) {
	glClearColor(0.32f, 0.27f, 0.40f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	float time_value = glfwGetTime();
	GLfloat intensity = (sin(time_value) / 2.0f) + 0.5f;

	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe mode

	state.uniforms.set_float1("intensity", intensity);

	state.shader_program->use();
	state.uniforms.apply_to_program(*state.shader_program);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, state.texture_0_id);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, state.texture_1_id);
	glBindVertexArray(state.vertex_array);
	static const GLsizei num_vertices = 3;
	glDrawElements(GL_TRIANGLES, num_vertices, GL_UNSIGNED_INT, NULL);

	// unbind to avoid accidental modification
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
