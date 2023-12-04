#include <iostream>

#include "hello_triangle.h"

using namespace glpg;

int hello_triangle::run() {
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

void hello_triangle::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void hello_triangle::process(GLFWwindow* window, State& state) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

void hello_triangle::initialize(GLFWwindow* window, State& state) {
	static const GLfloat vertices[] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f,
	};
	static const GLchar* vertex_shader_source =
		"#version 330 core\n"
		"layout (location = 0) in vec3 a_position;\n"
		"void main() { gl_Position = vec4(a_position.x, a_position.y, a_position.z, 1.0); }\0";
	static const GLchar* fragment_shader_source =
		"#version 330 core\n"
		"out vec4 frag_color;\n"
		"void main() { frag_color = vec4(1.0f, 0.5f, 0.2f, 1.0f); }\0";
	static const GLint position_attrib_index = 0; // layout (location = 0)

	// used to receive error messages later on
	GLint success;
	GLchar message[1024];

	GLuint vertex_buffer = 0;
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// any subsequent vertex attribute calls will be stored inside the vertex array
	glGenVertexArrays(1, &state.vertex_array);
	glBindVertexArray(state.vertex_array);

	glVertexAttribPointer(position_attrib_index, 3, GL_FLOAT, false, 0, (GLvoid*)0);
	glEnableVertexAttribArray(position_attrib_index);

	GLuint vertex_shader;
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
	glCompileShader(vertex_shader);
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex_shader, 1024, NULL, message);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << message << std::endl;
	}

	GLuint fragment_shader;
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
	glCompileShader(fragment_shader);
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment_shader, 1024, NULL, message);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << message << std::endl;
	}

	state.program = glCreateProgram();
	glAttachShader(state.program, vertex_shader);
	glAttachShader(state.program, fragment_shader);
	glLinkProgram(state.program);
	glGetProgramiv(state.program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(state.program, 1024, NULL, message);
		std::cout << "ERROR::PROGRAM::LINKAGE_FAILED\n" << message << std::endl;
	}

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
}

void hello_triangle::render(GLFWwindow* window, State& state) {
	glClearColor(0.32f, 0.27f, 0.40f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// do this every frame
	glUseProgram(state.program);
	glBindVertexArray(state.vertex_array);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}
