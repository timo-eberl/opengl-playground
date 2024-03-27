#include "meshes.h"

#include <mikktspace.h>

// static int get_vertex_index

using namespace ron;

static int get_vertex_index(const SMikkTSpaceContext *context, int face_idx, int vert_idx);

static int get_num_faces(const SMikkTSpaceContext *context);
static int get_num_vertices_of_face(const SMikkTSpaceContext *context, int face_idx);
static void get_position(
	const SMikkTSpaceContext *context, float out_position[], int face_idx, int vert_idx
);
static void get_normal(
	const SMikkTSpaceContext *context, float out_normal[], int face_idx, int vert_idx
);
static void get_uv(const SMikkTSpaceContext *context, float out_uv[], int face_idx, int vert_idx);
static void set_tangent_space(
	const SMikkTSpaceContext *context, const float tangent[], float sign,
	int face_idx, int vert_idx
);

struct MikktUserData {
	const Geometry &geometry;
	std::vector<glm::vec4> tangents = {};
};

std::vector<glm::vec4> ron::generate_tangents(const Geometry &geometry) {
	assert(geometry.positions.size() != 0);
	assert(geometry.positions.size() == geometry.normals.size());
	assert(geometry.positions.size() == geometry.uvs.size());

	MikktUserData user_data = { geometry, {} };
	user_data.tangents.resize(geometry.positions.size());

	SMikkTSpaceInterface mikkt_interface = {};
	SMikkTSpaceContext mikkt_context = {};

	mikkt_interface.m_getNumFaces = get_num_faces;
	mikkt_interface.m_getNumVerticesOfFace = get_num_vertices_of_face;
	mikkt_interface.m_getPosition = get_position;
	mikkt_interface.m_getNormal = get_normal;
	mikkt_interface.m_getTexCoord = get_uv;
	mikkt_interface.m_setTSpaceBasic = set_tangent_space;

	mikkt_context.m_pInterface = &mikkt_interface;
	mikkt_context.m_pUserData = &user_data;

	genTangSpaceDefault(&mikkt_context);

	return user_data.tangents;
}

int get_vertex_index(const SMikkTSpaceContext *context, int face_idx, int vert_idx) {
	auto &geometry = static_cast<MikktUserData *>(context->m_pUserData)->geometry;

	const auto vert_per_face = get_num_vertices_of_face(context, face_idx);
	const auto indices_index = (face_idx * vert_per_face) + vert_idx;

	return geometry.indices[indices_index];
}

int get_num_faces(const SMikkTSpaceContext *context) {
	auto &geometry = static_cast<MikktUserData *>(context->m_pUserData)->geometry;
	return geometry.indices.size() / 3;
}

int get_num_vertices_of_face(
#if defined(__GNUC__)
	// mark parameters as unused to suppress warnings
	const SMikkTSpaceContext* context __attribute__((unused)), int face_idx __attribute__((unused))
#else
	const SMikkTSpaceContext* context, int face_idx
#endif
) {
	return 3;
}

void get_position(
	const SMikkTSpaceContext *context, float out_position[], int face_idx, int vert_idx
) {
	auto &geometry = static_cast<MikktUserData *>(context->m_pUserData)->geometry;
	auto position = geometry.positions[get_vertex_index(context, face_idx, vert_idx)];

	out_position[0] = position.x;
	out_position[1] = position.y;
	out_position[2] = position.z;
}

void get_normal(
	const SMikkTSpaceContext *context, float out_normal[], int face_idx, int vert_idx
) {
	auto &geometry = static_cast<MikktUserData *>(context->m_pUserData)->geometry;
	auto normal = geometry.normals[get_vertex_index(context, face_idx, vert_idx)];

	out_normal[0] = normal.x;
	out_normal[1] = normal.y;
	out_normal[2] = normal.z;
}

void get_uv(const SMikkTSpaceContext *context, float out_uv[], int face_idx, int vert_idx) {
	auto &geometry = static_cast<MikktUserData *>(context->m_pUserData)->geometry;
	auto uv = geometry.uvs[get_vertex_index(context, face_idx, vert_idx)];

	out_uv[0] = uv.x;
	out_uv[1] = uv.y;
}

void set_tangent_space(
	const SMikkTSpaceContext *context, const float tangent[], float sign,
	int face_idx, int vert_idx
) {
	auto user_data = static_cast<MikktUserData *>(context->m_pUserData);

	auto index = get_vertex_index(context, face_idx, vert_idx);
	user_data->tangents[index].x = tangent[0];
	user_data->tangents[index].y = tangent[1];
	user_data->tangents[index].z = tangent[2];
	// for some reason the sign has to be flippid, probably a handedness messup
	user_data->tangents[index].w = -sign;
}
