#version 330 core

in vec3 normal;
in vec2 uv;

out vec4 frag_color;

void main() {
	frag_color = vec4(uv.xy, 0.0, 1.0);
	vec3 n = normalize(normal);
	frag_color = vec4(normal.xyz, 1.0);
}
