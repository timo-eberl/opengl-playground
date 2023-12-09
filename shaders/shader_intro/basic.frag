#version 330 core

in vec3 vertex_color;

out vec4 frag_color;

uniform float intensity;
uniform vec3 min_color;

void main() {
	frag_color = vec4(max(vertex_color * intensity, min_color), 1.0);
}
