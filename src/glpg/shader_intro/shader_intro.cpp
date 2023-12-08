#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

#include "shader_intro.h"

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

void shader_intro::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void shader_intro::process(GLFWwindow* window, State& state) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

void shader_intro::initialize(GLFWwindow* window, State& state) {
	static const GLfloat vertices[] = {
		// positions        	// colors
		 0.0f,   0.5f, 0.0f,	1.0f, 1.0f, 0.0f, // top center
		 0.5f,  -0.5f, 0.0f,	0.0f, 1.0f, 1.0f, // bottom right
		-0.5f,  -0.5f, 0.0f,	1.0f, 0.0f, 1.0f, // bottom left
	};
	static const GLuint indices[] = {
		0, 1, 2,
	};

	std::string vert_s;
	auto file_path = "shaders/shader_intro/basic.vert";
	std::ifstream ifs (file_path, std::ifstream::in);
	if (ifs) {
		std::stringstream string_stream;
		string_stream << ifs.rdbuf();
		vert_s = string_stream.str();
		ifs.close();
	}
	else {
		std::cerr << "\033[1;31m";
		std::cerr << "Can't read file " << file_path << std::endl;
		std::cerr << "\033[1;0m";
		return;
	}
	static const GLchar* vertex_shader_source = vert_s.c_str();

	std::string frag_s;
	file_path = "shaders/shader_intro/basic.frag";
	ifs = std::ifstream(file_path, std::ifstream::in);
	if (ifs) {
		std::stringstream string_stream;
		string_stream << ifs.rdbuf();
		frag_s = string_stream.str();
		ifs.close();
	}
	else {
		std::cerr << "\033[1;31m";
		std::cerr << "error rading file \"" << file_path << '\"' << std::endl;
		std::cerr << "\033[1;0m";
		return;
	}
	static const GLchar* fragment_shader_source = frag_s.c_str();

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

	GLuint vertex_shader;
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
	glCompileShader(vertex_shader);
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex_shader, 1024, NULL, message);
		std::cerr << "\033[1;31m";
		std::cerr << "vertex shader compilation failed\n" << message << std::endl;
		std::cerr << "\033[1;0m";
	}

	GLuint fragment_shader;
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
	glCompileShader(fragment_shader);
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment_shader, 1024, NULL, message);
		std::cerr << "\033[1;31m";
		std::cerr << "fragment shader compilation failed\n" << message << std::endl;
		std::cerr << "\033[1;0m";
	}

	state.program = glCreateProgram();
	glAttachShader(state.program, vertex_shader);
	glAttachShader(state.program, fragment_shader);
	glLinkProgram(state.program);
	glGetProgramiv(state.program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(state.program, 1024, NULL, message);
		std::cerr << "\033[1;31m";
		std::cerr << "program linkage failed\n" << message << std::endl;
		std::cerr << "\033[1;0m";
	}

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

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
	auto intensity_location = glGetUniformLocation(state.program, "intensity");

	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe mode

	glUseProgram(state.program);
	glUniform1f(intensity_location, intensity);
	glBindVertexArray(state.vertex_array);
	static const GLsizei num_vertices = 3;
	glDrawElements(GL_TRIANGLES, num_vertices, GL_UNSIGNED_INT, NULL);

	// unbind vertex array to avoid accidental modification
	glBindVertexArray(0);
}
