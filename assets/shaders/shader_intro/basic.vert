#version 330 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_color;

out vec3 vertex_color;

void main() {
	gl_Position = vec4(a_position.x, a_position.y, a_position.z, 1.0);
	vertex_color = a_color;
}
