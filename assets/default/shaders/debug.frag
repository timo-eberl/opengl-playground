#version 330 core

out vec4 frag_color;

in vec2 uv;

uniform sampler2D tex;

void main() {
	vec4 t = texture(tex, uv);
	frag_color = vec4(t.xxx, 1.0);
}
