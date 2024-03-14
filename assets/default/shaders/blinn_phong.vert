#version 330 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_uv;
layout (location = 3) in vec4 a_tangent;

out vec3 world_normal;
out vec3 world_position;
out vec2 uv;
out vec4 tangent;

uniform mat4 model_matrix;
uniform mat4 view_projection_matrix;
uniform mat3 normal_local_to_world_matrix;

void main() {
	world_position = vec3(model_matrix * vec4(a_position, 1.0));
	gl_Position = view_projection_matrix * vec4(world_position, 1.0);
	uv = a_uv;
	tangent.xyz = mat3(model_matrix) * a_tangent.xyz;
	tangent.w = a_tangent.w;
	world_normal = normal_local_to_world_matrix * a_normal;
}
