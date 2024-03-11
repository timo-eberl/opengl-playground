#version 330 core

in vec3 world_normal;
in vec3 world_position;
in vec2 uv;

out vec4 frag_color;

uniform vec3 camera_world_position;
uniform vec3 directional_light_world_direction;
uniform vec4 albedo_color;
uniform float metallic_factor;
uniform float roughness_factor;
uniform sampler2D albedo_tex;
uniform sampler2D metallic_roughness_tex;
uniform sampler2D normal_tex;

void main() {
	vec4 t_albedo_tex = texture(albedo_tex, uv);
	vec4 t_metallic_roughness_tex = texture(metallic_roughness_tex, uv);
	vec4 t_normal_tex = texture(normal_tex, uv);

	float metallic = t_metallic_roughness_tex.b * metallic_factor;
	float roughness = t_metallic_roughness_tex.g * roughness_factor;
	vec3 albedo = t_albedo_tex.rgb * albedo_color.rgb;
	vec3 normal = normalize(world_normal);

	// lambertian diffuse
	float diffuse_lighting = clamp( dot(directional_light_world_direction, normal), 0,1 );

	float ambient_lighting = 0.2;

	vec3 view_direction = normalize(camera_world_position - world_position);
	vec3 half_vector = normalize(directional_light_world_direction + view_direction);
	// blinn-phong specular lighting
	float specular_lighting = clamp( dot(half_vector, normal), 0,1 );
	specular_lighting = pow(specular_lighting, (1.0 - roughness) * 100.0);

	float specular_strength = 0.5;
	float directional_light_intensity = 0.75;

	frag_color = vec4(vec3(
		mix(diffuse_lighting * albedo, vec3(specular_lighting), specular_strength)
		* directional_light_intensity + vec3(ambient_lighting * albedo)
	), 1.0);
}
