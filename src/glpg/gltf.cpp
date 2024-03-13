#include "gltf.h"

#include <glad/glad.h> // for sampler constants

#include <cgltf.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cassert>

#include "log.h"

#define ASSETS_DIR _ASSETS_DIR

using namespace glpg;

static void extract_attributes(
	const cgltf_primitive &primitive,
	cgltf_accessor **pos_attribute, cgltf_accessor **normal_attribute,
	cgltf_accessor **uv_attribute, cgltf_accessor **tangent_attribute
) {
	for (size_t i = 0; i < primitive.attributes_count; i++) {
		const auto name = std::string(primitive.attributes[i].name);
		if (name == std::string("POSITION")) {
			*pos_attribute = primitive.attributes[i].data;
		}
		else if (name == std::string("NORMAL")) {
			*normal_attribute = primitive.attributes[i].data;
		}
		else if (name == std::string("TEXCOORD_0")) {
			*uv_attribute = primitive.attributes[i].data;
		}
		else if (name == std::string("TANGENT")) {
			*tangent_attribute = primitive.attributes[i].data;
		}
	}
}

static bool is_primitive_valid(const cgltf_primitive &primitive, cgltf_node *node, std::vector<std::string> &unsupported) {
	if (primitive.type != cgltf_primitive_type_triangles) {
		unsupported.push_back("Mesh type other than primitive triangles");
	}
	if (!primitive.indices) {
		unsupported.push_back("Non-indexed mesh");
	}
	if (primitive.indices->component_type != cgltf_component_type_r_16u // unsigned short
		&& primitive.indices->component_type != cgltf_component_type_r_32u // unsigned int
	) {
		unsupported.push_back("Index format other than unsigned short or unsigned int");
	}

	cgltf_accessor *pos_attribute = nullptr;
	cgltf_accessor *normal_attribute = nullptr; // optional
	cgltf_accessor *uv_attribute = nullptr; // optional
	cgltf_accessor *tangent_attribute = nullptr; // optional
	extract_attributes(
		primitive, &pos_attribute, &normal_attribute, &uv_attribute, &tangent_attribute
	);

	if (pos_attribute->component_type != cgltf_component_type_r_32f ||
		(normal_attribute && normal_attribute->component_type != cgltf_component_type_r_32f) ||
		(uv_attribute && uv_attribute->component_type != cgltf_component_type_r_32f) ||
		(tangent_attribute && tangent_attribute->component_type != cgltf_component_type_r_32f)
	) {
		unsupported.push_back("Attribute type other than 32 bit float");
	}
	if (pos_attribute->type != cgltf_type_vec3) {
		unsupported.push_back("Position attribute with a number of components other than 3");
	}
	if (normal_attribute && normal_attribute->type != cgltf_type_vec3) {
		unsupported.push_back("Normal attribute with a number of components other than 3");
	}
	if (uv_attribute && uv_attribute->type != cgltf_type_vec2) {
		unsupported.push_back("Texture Coordinate attribute with a number of components other than 2");
	}
	if (tangent_attribute && tangent_attribute->type != cgltf_type_vec4) {
		unsupported.push_back("Tangent attribute with a number of components other than 4");
	}

	return unsupported.size() == 0;
}

static Texture::SampleData create_sample_data(
	const cgltf_sampler &sampler, std::vector<std::string> &unsupported
) {
	Texture::SampleData sample_data = {};
	
	switch (sampler.wrap_s) {
		case GL_CLAMP_TO_EDGE: sample_data.wrap_mode_s = Texture::CLAMP_TO_EDGE; break;
		case GL_CLAMP_TO_BORDER: sample_data.wrap_mode_s = Texture::CLAMP_TO_BORDER; break;
		case GL_MIRRORED_REPEAT: sample_data.wrap_mode_s = Texture::MIRRORED_REPEAT; break;
		case GL_REPEAT: sample_data.wrap_mode_s = Texture::REPEAT; break;
		case GL_MIRROR_CLAMP_TO_EDGE: sample_data.wrap_mode_s = Texture::MIRROR_CLAMP_TO_EDGE; break;
		default: sample_data.wrap_mode_s = Texture::REPEAT; break;
	}
	switch (sampler.wrap_t) {
		case GL_CLAMP_TO_EDGE: sample_data.wrap_mode_t = Texture::CLAMP_TO_EDGE; break;
		case GL_CLAMP_TO_BORDER: sample_data.wrap_mode_t = Texture::CLAMP_TO_BORDER; break;
		case GL_MIRRORED_REPEAT: sample_data.wrap_mode_t = Texture::MIRRORED_REPEAT; break;
		case GL_REPEAT: sample_data.wrap_mode_t = Texture::REPEAT; break;
		case GL_MIRROR_CLAMP_TO_EDGE: sample_data.wrap_mode_t = Texture::MIRROR_CLAMP_TO_EDGE; break;
		default: sample_data.wrap_mode_t = Texture::REPEAT; break;
	}
	switch (sampler.min_filter) {
		case GL_NEAREST: sample_data.min_filter = Texture::NEAREST; break;
		case GL_LINEAR: sample_data.min_filter = Texture::LINEAR; break;
		case GL_NEAREST_MIPMAP_NEAREST: sample_data.min_filter = Texture::NEAREST_MIPMAP_NEAREST; break;
		case GL_LINEAR_MIPMAP_NEAREST: sample_data.min_filter = Texture::LINEAR_MIPMAP_NEAREST; break;
		case GL_NEAREST_MIPMAP_LINEAR: sample_data.min_filter = Texture::NEAREST_MIPMAP_LINEAR; break;
		case GL_LINEAR_MIPMAP_LINEAR: sample_data.min_filter = Texture::LINEAR_MIPMAP_LINEAR; break;
		default: sample_data.min_filter = Texture::LINEAR_MIPMAP_LINEAR; break;
	}
	switch (sampler.mag_filter) {
		case GL_NEAREST: sample_data.mag_filter = Texture::MAG_NEAREST; break;
		case GL_LINEAR: sample_data.mag_filter = Texture::MAG_LINEAR; break;
		default: sample_data.mag_filter = Texture::MAG_LINEAR; break;
	}

	return sample_data;
}

static std::shared_ptr<Texture> create_texture(
	const cgltf_texture_view &gltf_texture_view, std::vector<std::string> &unsupported,
	const std::string &gltf_path, bool srgb = true
) {
	assert(!gltf_texture_view.has_transform);
	assert(gltf_texture_view.texcoord == 0);
	const auto image = gltf_texture_view.texture->image;

	const auto sample_data = gltf_texture_view.texture->sampler
		? create_sample_data(*gltf_texture_view.texture->sampler, unsupported)
		: Texture::SampleData();

	if (image->buffer_view) {
		const unsigned char *raw_data = cgltf_buffer_view_data(image->buffer_view);
		const int raw_data_len = image->buffer_view->size;

		const auto image_data = Texture::image_data_from_memory(raw_data, raw_data_len, false);
		assert(image_data.data_ptr);

		return std::make_shared<Texture>(
			image_data,
			image->name ? image->name : "",
			Texture::MetaData(
				Texture::Channels::AUTOMATIC,
				srgb ? Texture::ColorSpace::SRGB : Texture::ColorSpace::NON_COLOR,
				false
			),
			sample_data
		);
	}
	else {
		const bool path_contains_slash = gltf_path.find('/') != std::string::npos;
		const auto after_last_slash = path_contains_slash ? (gltf_path.find_last_of('/') + 1) : 0;
		const auto gltf_location = gltf_path.substr(0, after_last_slash);
		const auto asset_path = gltf_location + image->uri;
		const auto texture = std::make_shared<Texture>(asset_path, Texture::MetaData(
			Texture::Channels::AUTOMATIC,
			srgb ? Texture::ColorSpace::SRGB : Texture::ColorSpace::NON_COLOR,
			false
		), sample_data);

		return texture;
	}
}

static std::shared_ptr<Material> create_material(
	cgltf_material *gltf_material, std::vector<std::string> &unsupported,
	const std::string &gltf_path
) {
	if (!gltf_material->has_pbr_metallic_roughness) {
		unsupported.push_back("Material without pbrMetallicRoughness");
		return nullptr;
	}

	const auto material = std::make_shared<Material>();
	static const auto shader_program = std::make_shared<ShaderProgram>(
		"shaders/default/blinn_phong.vert", "shaders/default/blinn_phong.frag"
	);
	material->shader_program = shader_program;

	const auto &normal_tex = gltf_material->normal_texture;
	if (normal_tex.texture) {
		const auto texture = create_texture(normal_tex, unsupported, gltf_path, false);
		material->uniforms["normal_tex"] = make_uniform(texture);
	}
	else {
		static const auto fallback_normal = std::make_shared<Texture>(
			"textures/fallback/normal.png",
			Texture::MetaData(Texture::Channels::RGBA, Texture::ColorSpace::NON_COLOR, true)
		);
		material->uniforms["normal_tex"] = glpg::make_uniform(fallback_normal);
	}

	const auto &albedo_color = gltf_material->pbr_metallic_roughness.base_color_factor;
	material->uniforms["albedo_color"] = make_uniform(glm::vec4(
		albedo_color[0], albedo_color[1], albedo_color[2], albedo_color[3]
	));
	const auto &albedo_tex = gltf_material->pbr_metallic_roughness.base_color_texture;
	if (albedo_tex.texture) {
		const auto texture = create_texture(albedo_tex, unsupported, gltf_path);
		material->uniforms["albedo_tex"] = make_uniform(texture);
	}
	else {
		static const auto fallback_albedo
			= std::make_shared<Texture>("textures/fallback/white.jpg");
		material->uniforms["albedo_tex"] = glpg::make_uniform(fallback_albedo);
	}

	const auto &metallic_factor = gltf_material->pbr_metallic_roughness.metallic_factor;
	const auto &roughness_factor = gltf_material->pbr_metallic_roughness.roughness_factor;
	material->uniforms["metallic_factor"] = make_uniform(glm::vec1(metallic_factor));
	material->uniforms["roughness_factor"] = make_uniform(glm::vec1(roughness_factor));
	const auto &metallic_roughness_tex = gltf_material->pbr_metallic_roughness.metallic_roughness_texture;
	if (metallic_roughness_tex.texture) {
		const auto texture = create_texture(metallic_roughness_tex, unsupported, gltf_path, false);
		material->uniforms["metallic_roughness_tex"] = make_uniform(texture);
	}
	else {
		static const auto fallback_metallic_roughness = std::make_shared<Texture>(
			"textures/fallback/white.jpg",
			Texture::MetaData(Texture::Channels::RGB, Texture::ColorSpace::NON_COLOR, true)
		);
		material->uniforms["metallic_roughness_tex"]
			= glpg::make_uniform(fallback_metallic_roughness);
	}

	return material;
}

static void add_mesh_from_node(
	cgltf_node *node, Mesh &out_mesh, std::vector<std::string> &unsupported,
	const std::string &gltf_path
) {
	for (size_t i = 0; i < node->mesh->primitives_count; i++) {
		auto primitive = node->mesh->primitives[i];

		std::vector<std::string> unsupported_new;
		if (!is_primitive_valid(primitive, node, unsupported_new)) {
			for (auto &new_error : unsupported_new) {
				const auto node_name = node->name ? node->name : "";
				const auto mesh_name = node->mesh->name ? node->mesh->name : "";
				new_error = new_error + " (" + node_name + "." + mesh_name + ")";
			}
			unsupported.insert(unsupported.end(), unsupported_new.begin(), unsupported_new.end());
			continue;
		}

		auto geometry = glpg::Geometry();

		// load index data from storage buffer into geometry data
		switch (primitive.indices->component_type) {
			case cgltf_component_type_r_16u: { // unsigned short / uint16_t
				// read uint16_t values
				uint16_t indices_buffer [primitive.indices->count];
				cgltf_accessor_unpack_indices(
					primitive.indices, indices_buffer, sizeof(uint16_t), primitive.indices->count
				);
				// insert into geometry data -> automatically convert to uint32_t
				geometry.indices.reserve(primitive.indices->count);
				for (const auto & index : indices_buffer) { geometry.indices.push_back(index); }
			} break;
			case cgltf_component_type_r_32u: { // unsigned int / uint32_t
				geometry.indices.resize(primitive.indices->count);
				// directly copy the indices data into the vector
				cgltf_accessor_unpack_indices(
					primitive.indices, geometry.indices.data(), sizeof(uint32_t), primitive.indices->count
				);
			} break;
		}

		cgltf_accessor *pos_attribute = nullptr;
		cgltf_accessor *normal_attribute = nullptr; // optional
		cgltf_accessor *uv_attribute = nullptr; // optional
		cgltf_accessor *tangent_attribute = nullptr; // optional
		extract_attributes(
			primitive, &pos_attribute, &normal_attribute, &uv_attribute, &tangent_attribute
		);

		assert(pos_attribute); // position attribute must exist
		const auto vertices_count = pos_attribute->count;
		// others are optional, but if they exist, must have the same number of attributes
		assert(!normal_attribute || vertices_count == normal_attribute->count);
		assert(!uv_attribute || vertices_count == uv_attribute->count);
		assert(!tangent_attribute || vertices_count == tangent_attribute->count);

		// load attribute data from storage buffer into geometry data
		geometry.positions.resize(vertices_count);
		cgltf_accessor_unpack_floats(
			pos_attribute, reinterpret_cast<float *>(geometry.positions.data()), 3 * vertices_count
		);
		if (normal_attribute) {
			geometry.normals.resize(vertices_count);
			cgltf_accessor_unpack_floats(
				normal_attribute, reinterpret_cast<float *>(geometry.normals.data()), 3 * vertices_count
			);
		}
		if (uv_attribute) {
			geometry.uvs.resize(vertices_count);
			cgltf_accessor_unpack_floats(
				uv_attribute, reinterpret_cast<float *>(geometry.uvs.data()), 2 * vertices_count
			);
		}
		if (tangent_attribute) {
			geometry.tangents.resize(vertices_count);
			cgltf_accessor_unpack_floats(
				tangent_attribute, reinterpret_cast<float *>(geometry.tangents.data()), 4 * vertices_count
			);
		}

		// if no tangent attribute is present, calculate if possible (normals and uvs required)
		if (!tangent_attribute && normal_attribute && uv_attribute) {
			geometry.tangents = generate_tangents(geometry);
		}

		const auto material = create_material(primitive.material, unsupported, gltf_path);

		out_mesh.sections.push_back(MeshSection(
			std::make_shared<Geometry>(std::move(geometry)),
			material
		));
	}
}

static void add_all_meshes_from_node_recursive(
	cgltf_node *node, Scene &scene, std::vector<std::string> &unsupported, const std::string &gltf_path
) {
	if (node->mesh) {
		auto node_world_matrix = glm::identity<glm::mat4>();
		cgltf_node_transform_world(node, reinterpret_cast<float *>(&node_world_matrix));

		auto mesh = std::make_shared<Mesh>();
		add_mesh_from_node(node, *mesh, unsupported, gltf_path);
		scene.add(std::make_shared<MeshNode>(mesh, node_world_matrix));
	}
	for (size_t i = 0; i < node->children_count; i++) {
		add_all_meshes_from_node_recursive(node->children[i], scene, unsupported, gltf_path);
	}
}

static std::string result_to_string(cgltf_result result) {
	switch (result) {
		case cgltf_result_success: return "cgltf_result_success";
		case cgltf_result_data_too_short: return "cgltf_result_data_too_short";
		case cgltf_result_unknown_format: return "cgltf_result_unknown_format";
		case cgltf_result_invalid_json: return "cgltf_result_invalid_json";
		case cgltf_result_invalid_gltf: return "cgltf_result_invalid_gltf";
		case cgltf_result_invalid_options: return "cgltf_result_invalid_options";
		case cgltf_result_file_not_found: return "cgltf_result_file_not_found";
		case cgltf_result_io_error: return "cgltf_result_io_error";
		case cgltf_result_out_of_memory: return "cgltf_result_out_of_memory";
		case cgltf_result_legacy_gltf: return "cgltf_result_legacy_gltf";
		case cgltf_result_max_enum: return "cgltf_result_max_enum";
		default: return "unknown error";
	}
}

Scene gltf::import(const std::string& path) {
	std::string full_path = ASSETS_DIR + path;
	cgltf_options options = {};
	cgltf_data* data = nullptr;
	// parse gltf or glb file
	auto parse_result = cgltf_parse_file(&options, full_path.c_str(), &data);
	// load binary buffers
	auto load_buffers_result = cgltf_load_buffers(&options, data, full_path.c_str());

	Scene scene = {};

	if (parse_result != cgltf_result_success) {
		log::error("Importing glTF failed: " + result_to_string(parse_result) + " (" + path + ")");
		return scene;
	}
	else if (load_buffers_result != cgltf_result_success) {
		log::error("Loading glTF buffers failed: " + result_to_string(load_buffers_result) + " (" + path + ")");
		return scene;
	}

	std::vector<std::string> unsupported_features = {};
	for (size_t i = 0; i < data->scene->nodes_count; i++) {
		auto node = data->scene->nodes[i];
		add_all_meshes_from_node_recursive(node, scene, unsupported_features, path);
	}

	if (unsupported_features.size() > 0) {
		log::error("Incomplete glTF import. " + path + " uses unsupported features:");
		for (const auto &error : unsupported_features) {
			log::error("\t- " + error);
		}
	}
	else {
		log::success("glTF imported successfully (" + path + ")");
	}

	cgltf_free(data);

	return scene;
}
