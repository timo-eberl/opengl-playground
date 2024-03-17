#version 330 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in float a_color;

out vec3 world_position;
out float color;

uniform mat4 view_projection_matrix;
uniform vec3 camera_world_position;

void main() {
	// move the grid with the camera in steps of 10
	ivec2 offset_i = ivec2(camera_world_position.xz) / 10;
	vec3 offset = vec3(offset_i.x, 0, offset_i.y) * 10.0;

	world_position = a_position + offset;
	color = a_color;

	gl_Position = view_projection_matrix * vec4(world_position, 1.0);

}
