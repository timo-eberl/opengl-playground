#version 330 core

in vec3 color;
in vec3 world_position;

out vec4 frag_color;

uniform vec3 camera_world_position;

void main() {
	float alpha = 1 - (distance(camera_world_position, world_position) / 500.0);
	frag_color = vec4(color, alpha);
}
