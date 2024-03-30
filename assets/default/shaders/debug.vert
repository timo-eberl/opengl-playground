#version 330 core

layout (location = 0) in vec2 a_position;

out vec2 uv;

void main() {
	gl_Position = vec4(a_position, 0.0, 1.0);
	uv = gl_Position.xy * 0.5 + 0.5;
}
