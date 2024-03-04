#pragma once

#include <glad/glad.h>
// include glfw after glad
#include <GLFW/glfw3.h>

#include <vector>
#include <memory>
#include <cstdint>

#include "geometry_data.h"

namespace glpg::meshes {

class Geometry {
public:
	Geometry(const GeometryData geometry_data);
	~Geometry();
	// forbid copying, because it would be probably not what we want
	Geometry(const Geometry&) = delete;
	Geometry &operator=(const Geometry&) = delete;

	void draw() const;
private:
	GeometryData m_geometry_data;
	GLuint m_vertex_array = 0;
	GLuint m_vertex_buffer = 0;
	GLuint m_index_buffer = 0;

	void setup();
	void release();
};

} // glpg::meshes
