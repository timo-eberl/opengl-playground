#version 330 core

in vec3 world_position;
in float color;

out vec4 frag_color;

uniform vec3 camera_world_position;

void main() {
	float alpha = 1 - (distance(camera_world_position, world_position) * 0.005);
	frag_color = vec4(vec3(color), alpha);
}
