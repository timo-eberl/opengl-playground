#version 330 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_uv;

out vec3 world_normal;
out vec3 world_position;
out vec2 uv;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;
uniform mat4 view_projection_matrix;
uniform mat3 normal_local_to_world_matrix;

void main() {
	world_position = vec3(model_matrix * vec4(a_position.x, a_position.y, a_position.z, 1.0));
	gl_Position = view_projection_matrix * vec4(world_position, 1.0);
	uv = a_uv;
	world_normal = normal_local_to_world_matrix * a_normal;
}
