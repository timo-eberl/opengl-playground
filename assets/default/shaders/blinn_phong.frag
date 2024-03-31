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
uniform bool directional_light_shadow_enabled;
uniform float directional_light_shadow_bias;
uniform vec4 albedo_color;
uniform float metallic_factor;
uniform float roughness_factor;
uniform sampler2D albedo_tex;
uniform sampler2D metallic_roughness_tex;
uniform sampler2D normal_tex;
uniform sampler2DShadow shadow_map;

const float poisson_disk_spread = 0.002;
const int poisson_num_samples = 16;
vec2 poisson_disk[16] = vec2[](
	vec2(-0.94201624,  -0.39906216),
	vec2( 0.94558609,  -0.76890725),
	vec2(-0.094184101, -0.92938870),
	vec2( 0.34495938,   0.29387760),
	vec2(-0.91588581,   0.45771432),
	vec2(-0.81544232,  -0.87912464),
	vec2(-0.38277543,   0.27676845),
	vec2( 0.97484398,   0.75648379),
	vec2( 0.44323325,  -0.97511554),
	vec2( 0.53742981,  -0.47373420),
	vec2(-0.26496911,  -0.41893023),
	vec2( 0.79197514,   0.19090188),
	vec2(-0.24188840,   0.99706507),
	vec2(-0.81409955,   0.91437590),
	vec2( 0.19984126,   0.78641367),
	vec2( 0.14383161,  -0.14100790)
);

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

void prepare_shadow_map_samples(
	out bool exit_early, out float exit_early_one_minus_shadow,
	out vec2[poisson_num_samples] poisson_uvs, out float depth_minus_bias
) {
	// transform from light_space (clip space) to normalized device coordinates
	// the GPU automatically does this for gl_Position and we have to accomodate for that
	// meaningless for orthographic projection, but important for perspective projection
	vec3 light_space_ndc_position = light_space_position.xyz / light_space_position.w;
	// transform from [-1,1] to [0,1]
	vec3 light_space_ndc_01_position = light_space_ndc_position * 0.5 + 0.5;
	vec2 uv = light_space_ndc_01_position.xy;

	float current_depth = light_space_ndc_01_position.z;
	// limit the depth to 1.0, otherwise areas that are behind the lights frustum are shadowed
	current_depth = min(current_depth, 1.0);

	// increase bias the steeper the angle of the light
	float bias = max(
		directional_light_shadow_bias * (1.0 - dot(world_normal,directional_light_world_direction)),
		directional_light_shadow_bias * 0.01
	);
	depth_minus_bias = current_depth - bias;

	// make the offset dependent on the texture size, because
	// if the resolution is too low we get aliasing, if it is too high we get banding
	vec2 uv_offset_multiplier = poisson_disk_spread * (1024.0 / textureSize(shadow_map, 0));

	// instead of taking a lot of samples whenpoisson sampling, we may be able to exit early,
	// if the whole area that would be sampled is in the light or in shadow.
	// to approximate this we take samples at the corners of the area.
	// there are cases where the approximation is incorrect
	vec2[4] exit_early_uvs;
	exit_early_uvs[0] = uv + vec2(-1.0, -1.0) * uv_offset_multiplier;
	exit_early_uvs[1] = uv + vec2( 1.0, -1.0) * uv_offset_multiplier;
	exit_early_uvs[2] = uv + vec2(-1.0,  1.0) * uv_offset_multiplier;
	exit_early_uvs[3] = uv + vec2( 1.0,  1.0) * uv_offset_multiplier;

	float[5] early_samples = {
		texture(shadow_map, vec3(uv, depth_minus_bias)),
		texture(shadow_map, vec3(exit_early_uvs[0], depth_minus_bias)),
		texture(shadow_map, vec3(exit_early_uvs[1], depth_minus_bias)),
		texture(shadow_map, vec3(exit_early_uvs[2], depth_minus_bias)),
		texture(shadow_map, vec3(exit_early_uvs[3], depth_minus_bias))
	};
	if (
		(early_samples[0] == 0.0 || early_samples[0] == 1.0)
		&& early_samples[0] == early_samples[1]
		&& early_samples[0] == early_samples[2]
		&& early_samples[0] == early_samples[3]
		&& early_samples[0] == early_samples[4]
	) {
		exit_early = true;
		exit_early_one_minus_shadow = early_samples[0];
		return;
	}

	for (int i = 0; i < poisson_num_samples; ++i) {
		poisson_uvs[i] = uv + poisson_disk[i] * uv_offset_multiplier;
	}
}

float poisson_sample_shadow_map(
	vec2[poisson_num_samples] poisson_uvs, float depth_minus_bias
) {
	float one_minus_shadow = 0.0;
	for (int i = 0; i < poisson_num_samples; ++i) {
		// shadow_map is a depth texture, therefore we sample it with a shadow sampler
		// the result is a single float value in the range [0,1]
		one_minus_shadow += texture(
			shadow_map, vec3(poisson_uvs[i], depth_minus_bias)
		);
	}
	return one_minus_shadow / float(poisson_num_samples);
}

struct Surface {
	vec3 albedo;
	vec3 normal;
	float metallic;
	float roughness;
};

vec3 blinn_phong_lighting(
	Surface surface, float one_minus_shadow
) {
	// lambertian diffuse
	float diffuse_lighting = clamp( dot(directional_light_world_direction, surface.normal), 0,1 );
	if (directional_light_shadow_enabled) {
		diffuse_lighting *= one_minus_shadow;
	}

	// constant ambient lighting
	float ambient_lighting = 0.07;

	// blinn-phong specular lighting
	vec3 view_direction = normalize(camera_world_position - world_position);
	vec3 half_vector = normalize(directional_light_world_direction + view_direction);
	float specular_lighting = clamp( dot(half_vector, surface.normal), 0,1 );
	specular_lighting = pow(specular_lighting, (1.0 - surface.roughness) * 100.0);
	if (directional_light_shadow_enabled) {
		specular_lighting *= one_minus_shadow;
	}

	const float specular_strength = 0.1;

	return vec3(
		mix(diffuse_lighting * surface.albedo, vec3(specular_lighting), specular_strength)
		* directional_light_intensity
		+ vec3(ambient_lighting * surface.albedo)
	);
}

void main() {
	bool shadow_exit_early;
	float one_minus_shadow = 0.0;
	vec2[poisson_num_samples] poissoned_shadow_map_uvs;
	float depth_minus_bias;
	if (directional_light_shadow_enabled) {
		prepare_shadow_map_samples(
			shadow_exit_early, one_minus_shadow, poissoned_shadow_map_uvs, depth_minus_bias
		);
	}

	// sample all textures at the same time
	vec4 t_albedo_tex = texture(albedo_tex, uv);
	vec4 t_metallic_roughness_tex = texture(metallic_roughness_tex, uv);
	vec4 t_normal_tex = texture(normal_tex, uv);
	if (directional_light_shadow_enabled && !shadow_exit_early) {
		one_minus_shadow = poisson_sample_shadow_map(
			poissoned_shadow_map_uvs, depth_minus_bias
		);
	}

	Surface surface;
	surface.albedo = t_albedo_tex.rgb * albedo_color.rgb;
	surface.normal = initialize_normal(t_normal_tex);
	surface.metallic = t_metallic_roughness_tex.b * metallic_factor;
	surface.roughness = t_metallic_roughness_tex.g * roughness_factor;

	frag_color = vec4(
		blinn_phong_lighting(surface, one_minus_shadow),
	1.0);
}
