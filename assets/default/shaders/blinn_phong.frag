#version 460 core

in vec3 world_normal;
in vec3 world_position;
in vec4 light_space_position;
in vec2 uv;
in vec4 tangent;

out vec4 frag_color;

uniform vec3 camera_world_position;
uniform vec3 directional_light_world_direction;
uniform float directional_light_intensity;
uniform int directional_light_shadow_enabled;
uniform float directional_light_shadow_bias;
uniform vec4 albedo_color;
uniform float metallic_factor;
uniform float roughness_factor;
uniform sampler2D albedo_tex;
uniform sampler2D metallic_roughness_tex;
uniform sampler2D normal_tex;
uniform sampler2D shadow_map;

vec3 initialize_normal(vec4 t_normal_tex) {
	// swap y and z, convert to [-1;1] range
	vec3 tangent_space_normal = t_normal_tex.xzy * 2.0 - 1.0;

	vec3 binormal = cross(world_normal, tangent.xyz) * tangent.w;

	// convert from tangent space to world space
	vec3 normal = normalize(
		tangent_space_normal.x * tangent.xyz +
		tangent_space_normal.y * world_normal +
		tangent_space_normal.z * binormal
	);

	return normal;
}

void prepare_shadow_map_samples(out vec2[3][3] shadow_map_uv, out float current_depth) {
	// transform from light_space (clip space) to normalized device coordinates
	// the GPU automatically does this for gl_Position and we have to accomodate for that
	// meaningless for orthographic projection, but important for perspective projection
	vec3 light_space_ndc_position = light_space_position.xyz / light_space_position.w;

	// transform from [-1,1] to [0,1]
	vec3 light_space_ndc_01_position = light_space_ndc_position * 0.5 + 0.5;

	vec2 texel_size = 1.0 / textureSize(shadow_map, 0);

	for (int x = -1; x < 2; ++x)
	for (int y = -1; y < 2; ++y) {
		shadow_map_uv[x+1][y+1] = light_space_ndc_01_position.xy + vec2(x,y) * texel_size;
	}

	current_depth = light_space_ndc_01_position.z;
}

float calculate_shadow(float current_depth, vec4[3][3] shadow_map_samples) {
	// limit the depth to 1.0, otherwise areas that are behind the lights frustum are shadowed
	current_depth = min(current_depth, 1.0);
	// increase bias the steeper the angle of the light
	float bias = max(
		directional_light_shadow_bias * (1.0 - dot(world_normal,directional_light_world_direction)),
		directional_light_shadow_bias * 0.01
	);

	float shadow = 0.0;

	for (int x = 0; x < 3; ++x)
	for (int y = 0; y < 3; ++y) {
		float shadow_map_depth = shadow_map_samples[x][y].r;
		shadow += float(current_depth - bias > shadow_map_depth);
	}

	return shadow / 9.0;
}

void main() {
	vec2[3][3] shadow_map_uv;
	float current_depth;
	if (bool(directional_light_shadow_enabled)) {
		prepare_shadow_map_samples(shadow_map_uv, current_depth);
	}
	vec4[3][3] shadow_map_samples;

	// sample all textures at the same time
	vec4 t_albedo_tex = texture(albedo_tex, uv);
	vec4 t_metallic_roughness_tex = texture(metallic_roughness_tex, uv);
	vec4 t_normal_tex = texture(normal_tex, uv);
	if (bool(directional_light_shadow_enabled)) {
		for (int x = 0; x < 3; ++x)
		for (int y = 0; y < 3; ++y) {
			shadow_map_samples[x][y] = texture(shadow_map, shadow_map_uv[x][y]);
		}
	}

	// surface properties
	vec3 albedo = t_albedo_tex.rgb * albedo_color.rgb;
	vec3 normal = initialize_normal(t_normal_tex);
	float metallic = t_metallic_roughness_tex.b * metallic_factor;
	float roughness = t_metallic_roughness_tex.g * roughness_factor;

	float shadow = 0.0;
	if (bool(directional_light_shadow_enabled)) {
		shadow = calculate_shadow(current_depth, shadow_map_samples);
	}

	// lambertian diffuse
	float diffuse_lighting = clamp( dot(directional_light_world_direction, normal), 0,1 );
	diffuse_lighting *= (1-shadow);

	// constant ambient lighting
	float ambient_lighting = 0.07;

	// blinn-phong specular lighting
	vec3 view_direction = normalize(camera_world_position - world_position);
	vec3 half_vector = normalize(directional_light_world_direction + view_direction);
	float specular_lighting = clamp( dot(half_vector, normal), 0,1 );
	specular_lighting = pow(specular_lighting, (1.0 - roughness) * 100.0);
	specular_lighting *= (1-shadow);

	const float specular_strength = 0.1;

	frag_color = vec4(vec3(
		mix(diffuse_lighting * albedo, vec3(specular_lighting), specular_strength)
		* directional_light_intensity
		+ vec3(ambient_lighting * albedo)
	), 1.0);
}
