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
	// orange rectangle
	{
		static const GLfloat vertices[] = {
			 0.5f,  0.4f, 0.0f, // top right
			 0.5f, -0.5f, 0.0f, // bottom right
			-0.5f, -0.5f, 0.0f, // bottom left
			-0.5f,  0.4f, 0.0f, // top left
		};
		static const GLuint indices[] = {
			0, 1, 3, // first triangle
			1, 2, 3, // second triangle
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

		// any subsequent vertex attribute calls and bind buffer calls
		// will be stored inside the vertex array
		glGenVertexArrays(1, &state.vertex_array_0);
		glBindVertexArray(state.vertex_array_0);

		GLuint vertex_buffer = 0;
		glGenBuffers(1, &vertex_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		GLuint element_buffer = 0;
		glGenBuffers(1, &element_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

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

		state.program_0 = glCreateProgram();
		glAttachShader(state.program_0, vertex_shader);
		glAttachShader(state.program_0, fragment_shader);
		glLinkProgram(state.program_0);
		glGetProgramiv(state.program_0, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(state.program_0, 1024, NULL, message);
			std::cout << "ERROR::PROGRAM::LINKAGE_FAILED\n" << message << std::endl;
		}

		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);

		// unbind buffers to avoid accidental modification
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	// red triangle
	{
		static const GLfloat vertices[] = {
			 0.0f,  0.75f, 0.0f, // top center
			 0.5f,  0.4f,  0.0f, // bottom right
			-0.5f,  0.4f,  0.0f, // bottom left
		};
		static const GLuint indices[] = {
			0, 1, 2,
		};
		static const GLchar* vertex_shader_source =
			"#version 330 core\n"
			"layout (location = 0) in vec3 a_position;\n"
			"void main() { gl_Position = vec4(a_position.x, a_position.y, a_position.z, 1.0); }\0";
		static const GLchar* fragment_shader_source =
			"#version 330 core\n"
			"out vec4 frag_color;\n"
			"void main() { frag_color = vec4(0.8f, 0.3f, 0.3f, 1.0f); }\0";
		static const GLint position_attrib_index = 0; // layout (location = 0)

		// used to receive error messages later on
		GLint success;
		GLchar message[1024];

		// any subsequent vertex attribute calls and bind buffer calls
		// will be stored inside the vertex array
		glGenVertexArrays(1, &state.vertex_array_1);
		glBindVertexArray(state.vertex_array_1);

		GLuint vertex_buffer = 0;
		glGenBuffers(1, &vertex_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		GLuint element_buffer = 0;
		glGenBuffers(1, &element_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

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

		state.program_1 = glCreateProgram();
		glAttachShader(state.program_1, vertex_shader);
		glAttachShader(state.program_1, fragment_shader);
		glLinkProgram(state.program_1);
		glGetProgramiv(state.program_1, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(state.program_1, 1024, NULL, message);
			std::cout << "ERROR::PROGRAM::LINKAGE_FAILED\n" << message << std::endl;
		}

		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);

		// unbind buffers to avoid accidental modification
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

void hello_triangle::render(GLFWwindow* window, State& state) {
	glClearColor(0.32f, 0.27f, 0.40f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe mode

	// draw orange rectangle
	{
		glUseProgram(state.program_0);
		glBindVertexArray(state.vertex_array_0);
		static const GLsizei num_vertices = 6;
		glDrawElements(GL_TRIANGLES, num_vertices, GL_UNSIGNED_INT, NULL);

		// unbind vertex array to avoid accidental modification
		glBindVertexArray(0);
	}

	// draw red triangle
	{
		glUseProgram(state.program_1);
		glBindVertexArray(state.vertex_array_1);
		static const GLsizei num_vertices = 3;
		glDrawElements(GL_TRIANGLES, num_vertices, GL_UNSIGNED_INT, NULL);

		// unbind vertex array to avoid accidental modification
		glBindVertexArray(0);
	}
}
