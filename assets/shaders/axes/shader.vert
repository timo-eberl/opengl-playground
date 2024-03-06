#version 330 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_color;

out vec3 color;

uniform mat4 view_projection_matrix;

void main() {
	gl_Position = view_projection_matrix * vec4(a_position, 1.0);
	color = a_color;
}
