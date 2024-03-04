#version 330 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_uv;

out vec3 normal;
out vec2 uv;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

void main() {
	gl_Position = projection_matrix * view_matrix * model_matrix * vec4(a_position.x, a_position.y, a_position.z, 1.0);
	uv = a_uv;
	normal = a_normal;
}
