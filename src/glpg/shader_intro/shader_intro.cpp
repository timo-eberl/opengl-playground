#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

#include "shader_intro.h"
#include "glpg/shader_program.h"
#include "glpg/uniforms.h"

using namespace glpg;

int shader_intro::run() {
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

	{ // this scope ensures that "state" is destroyed before the opengl context is destroyed (glfwTerminate)
		State state {};
		initialize(window, state);

		while (!glfwWindowShouldClose(window)) {
			process(window, state);

			render(window, state);

			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}

	glfwTerminate();
	return 0;
}

void shader_intro::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void shader_intro::process(GLFWwindow* window, State& state) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS) {
		state.shader_program->recreate();
	}
}

void shader_intro::initialize(GLFWwindow* window, State& state) {
	state.shader_program = std::make_unique<ShaderProgram>(
		"shaders/shader_intro/basic.vert", "shaders/shader_intro/basic.frag");

	state.uniforms.set_float3("min_color", { 0.2f, 0.2f, 0.2f });

	static const GLfloat vertices[] = {
		// positions        	// colors
		 0.0f,   0.5f, 0.0f,	1.0f, 1.0f, 0.0f, // top center
		 0.5f,  -0.5f, 0.0f,	0.0f, 1.0f, 1.0f, // bottom right
		-0.5f,  -0.5f, 0.0f,	1.0f, 0.0f, 1.0f, // bottom left
	};
	static const GLuint indices[] = {
		0, 1, 2,
	};

	static const GLint position_attrib_index = 0; // layout (location = 0)
	static const GLint color_attrib_index = 1; // layout (location = 1)

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

	GLsizei vertex_stride = 6 * sizeof(GLfloat);
	glVertexAttribPointer(position_attrib_index, 3, GL_FLOAT, false, vertex_stride, (GLvoid*)0);
	glEnableVertexAttribArray(position_attrib_index);
	glVertexAttribPointer(color_attrib_index, 3, GL_FLOAT, false, vertex_stride, (GLvoid*)(3*sizeof(GLfloat)));
	glEnableVertexAttribArray(color_attrib_index);

	// unbind buffers to avoid accidental modification
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void shader_intro::render(GLFWwindow* window, State& state) {
	glClearColor(0.32f, 0.27f, 0.40f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	float time_value = glfwGetTime();
	GLfloat intensity = (sin(time_value) / 2.0f) + 0.5f;

	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe mode

	state.uniforms.set_float1("intensity", intensity);

	state.shader_program->use();
	state.uniforms.apply_to_program(*state.shader_program);
	glBindVertexArray(state.vertex_array);
	static const GLsizei num_vertices = 3;
	glDrawElements(GL_TRIANGLES, num_vertices, GL_UNSIGNED_INT, NULL);

	// unbind vertex array to avoid accidental modification
	glBindVertexArray(0);
}
