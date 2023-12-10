#version 330 core

in vec2 uv;

out vec4 frag_color;

uniform sampler2D tex0;
uniform sampler2D tex1;

void main() {
	frag_color = vec4(uv, 0.0, 1.0);
	vec4 tex_color_0 = texture(tex0, uv);
	vec4 tex_color_1 = texture(tex1, uv);
	frag_color = mix(tex_color_0, tex_color_1, tex_color_1.w);
}
