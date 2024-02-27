#version 330 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec2 a_uv;

out vec2 uv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	gl_Position = projection * view * model * vec4(a_position.x, a_position.y, a_position.z, 1.0);
	uv = a_uv;
}
