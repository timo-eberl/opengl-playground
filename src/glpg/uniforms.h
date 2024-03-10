#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader_program.h"
#include "texture.h"

#include <map>
#include <memory>
#include <string>

namespace glpg {

enum UniformType {
	FLOAT1, FLOAT2, FLOAT3, FLOAT4,
	INT1, INT2, INT3, INT4,
	UINT1, UINT2, UINT3, UINT4,
	MAT2, MAT3, MAT4,
	MAT2X3, MAT3X2, MAT2X4, MAT4X2, MAT3X4, MAT4X3
};

class IUniform {
public:
	virtual ~IUniform() {};

	virtual const void *value_ptr() const = 0;
	virtual const UniformType get_type() const = 0;
};

typedef std::map<const std::string, std::shared_ptr<IUniform>> Uniforms;

class Float1Uniform : public IUniform {
public:
	Float1Uniform(glm::vec1 v) : value(v) {}
	virtual ~Float1Uniform() {};
	virtual const void *value_ptr() const override { return reinterpret_cast<const void *>(&value); }
	virtual const UniformType get_type() const { return FLOAT1; }
	glm::vec1 value;
};

class Float2Uniform : public IUniform {
public:
	Float2Uniform(glm::vec2 v) : value(v) {}
	virtual ~Float2Uniform() {};
	virtual const void *value_ptr() const override { return reinterpret_cast<const void *>(&value); }
	virtual const UniformType get_type() const { return FLOAT2; }
	glm::vec2 value;
};

class Float3Uniform : public IUniform {
public:
	Float3Uniform(glm::vec3 v) : value(v) {}
	virtual ~Float3Uniform() {};
	virtual const void *value_ptr() const override { return reinterpret_cast<const void *>(&value); }
	virtual const UniformType get_type() const { return FLOAT3; }
	glm::vec3 value;
};

class Float4Uniform : public IUniform {
public:
	Float4Uniform(glm::vec4 v) : value(v) {}
	virtual ~Float4Uniform() {};
	virtual const void *value_ptr() const override { return reinterpret_cast<const void *>(&value); }
	virtual const UniformType get_type() const { return FLOAT4; }
	glm::vec4 value;
};

class Int1Uniform : public IUniform {
public:
	Int1Uniform(glm::ivec1 v) : value(v) {}
	virtual ~Int1Uniform() {};
	virtual const void *value_ptr() const override { return reinterpret_cast<const void *>(&value); }
	virtual const UniformType get_type() const { return INT1; }
	glm::ivec1 value;
};

class Int2Uniform : public IUniform {
public:
	Int2Uniform(glm::ivec2 v) : value(v) {}
	virtual ~Int2Uniform() {};
	virtual const void *value_ptr() const override { return reinterpret_cast<const void *>(&value); }
	virtual const UniformType get_type() const { return INT2; }
	glm::ivec2 value;
};

class Int3Uniform : public IUniform {
public:
	Int3Uniform(glm::ivec3 v) : value(v) {}
	virtual ~Int3Uniform() {};
	virtual const void *value_ptr() const override { return reinterpret_cast<const void *>(&value); }
	virtual const UniformType get_type() const { return INT3; }
	glm::ivec3 value;
};

class Int4Uniform : public IUniform {
public:
	Int4Uniform(glm::ivec4 v) : value(v) {}
	virtual ~Int4Uniform() {};
	virtual const void *value_ptr() const override { return reinterpret_cast<const void *>(&value); }
	virtual const UniformType get_type() const { return INT4; }
	glm::ivec4 value;
};

class UInt1Uniform : public IUniform {
public:
	UInt1Uniform(glm::uvec1 v) : value(v) {}
	virtual ~UInt1Uniform() {};
	virtual const void *value_ptr() const override { return reinterpret_cast<const void *>(&value); }
	virtual const UniformType get_type() const { return UINT1; }
	glm::uvec1 value;
};

class UInt2Uniform : public IUniform {
public:
	UInt2Uniform(glm::uvec2 v) : value(v) {}
	virtual ~UInt2Uniform() {};
	virtual const void *value_ptr() const override { return reinterpret_cast<const void *>(&value); }
	virtual const UniformType get_type() const { return UINT2; }
	glm::uvec2 value;
};

class UInt3Uniform : public IUniform {
public:
	UInt3Uniform(glm::uvec3 v) : value(v) {}
	virtual ~UInt3Uniform() {};
	virtual const void *value_ptr() const override { return reinterpret_cast<const void *>(&value); }
	virtual const UniformType get_type() const { return UINT3; }
	glm::uvec3 value;
};

class UInt4Uniform : public IUniform {
public:
	UInt4Uniform(glm::uvec4 v) : value(v) {}
	virtual ~UInt4Uniform() {};
	virtual const void *value_ptr() const override { return reinterpret_cast<const void *>(&value); }
	virtual const UniformType get_type() const { return UINT4; }
	glm::uvec4 value;
};

class Mat2Uniform : public IUniform {
public:
	Mat2Uniform(glm::mat2 v) : value(v) {}
	virtual ~Mat2Uniform() {};
	virtual const void *value_ptr() const override { return reinterpret_cast<const void *>(&value); }
	virtual const UniformType get_type() const { return MAT2; }
	glm::mat2 value;
};

class Mat3Uniform : public IUniform {
public:
	Mat3Uniform(glm::mat3 v) : value(v) {}
	virtual ~Mat3Uniform() {};
	virtual const void *value_ptr() const override { return reinterpret_cast<const void *>(&value); }
	virtual const UniformType get_type() const { return MAT3; }
	glm::mat3 value;
};

class Mat4Uniform : public IUniform {
public:
	Mat4Uniform(glm::mat4 v) : value(v) {}
	virtual ~Mat4Uniform() {}
	virtual const void *value_ptr() const override { return reinterpret_cast<const void *>(&value); }
	virtual const UniformType get_type() const { return MAT4; }
	glm::mat4 value;
};

class Mat2x3Uniform : public IUniform {
public:
	Mat2x3Uniform(glm::mat2x3 v) : value(v) {}
	virtual ~Mat2x3Uniform() {};
	virtual const void *value_ptr() const override { return reinterpret_cast<const void *>(&value); }
	virtual const UniformType get_type() const { return MAT2X3; }
	glm::mat2x3 value;
};

class Mat3x2Uniform : public IUniform {
public:
	Mat3x2Uniform(glm::mat3x2 v) : value(v) {}
	virtual ~Mat3x2Uniform() {};
	virtual const void *value_ptr() const override { return reinterpret_cast<const void *>(&value); }
	virtual const UniformType get_type() const { return MAT3X2; }
	glm::mat3x2 value;
};

class Mat2x4Uniform : public IUniform {
public:
	Mat2x4Uniform(glm::mat2x4 v) : value(v) {}
	virtual ~Mat2x4Uniform() {};
	virtual const void *value_ptr() const override { return reinterpret_cast<const void *>(&value); }
	virtual const UniformType get_type() const { return MAT2X4; }
	glm::mat2x4 value;
};

class Mat4x2Uniform : public IUniform {
public:
	Mat4x2Uniform(glm::mat4x2 v) : value(v) {}
	virtual ~Mat4x2Uniform() {};
	virtual const void *value_ptr() const override { return reinterpret_cast<const void *>(&value); }
	virtual const UniformType get_type() const { return MAT4X2; }
	glm::mat4x2 value;
};

class Mat3x4Uniform : public IUniform {
public:
	Mat3x4Uniform(glm::mat3x4 v) : value(v) {}
	virtual ~Mat3x4Uniform() {};
	virtual const void *value_ptr() const override { return reinterpret_cast<const void *>(&value); }
	virtual const UniformType get_type() const { return MAT3X4; }
	glm::mat3x4 value;
};

class Mat4x3Uniform : public IUniform {
public:
	Mat4x3Uniform(glm::mat4x3 v) : value(v) {}
	virtual ~Mat4x3Uniform() {};
	virtual const void *value_ptr() const override { return reinterpret_cast<const void *>(&value); }
	virtual const UniformType get_type() const { return MAT4X3; }
	glm::mat4x3 value;
};

std::shared_ptr<IUniform> make_uniform(glm::vec1 value);
std::shared_ptr<IUniform> make_uniform(glm::vec2 value);
std::shared_ptr<IUniform> make_uniform(glm::vec3 value);
std::shared_ptr<IUniform> make_uniform(glm::vec4 value);

std::shared_ptr<IUniform> make_uniform(glm::ivec1 value);
std::shared_ptr<IUniform> make_uniform(glm::ivec2 value);
std::shared_ptr<IUniform> make_uniform(glm::ivec3 value);
std::shared_ptr<IUniform> make_uniform(glm::ivec4 value);

std::shared_ptr<IUniform> make_uniform(glm::uvec1 value);
std::shared_ptr<IUniform> make_uniform(glm::uvec2 value);
std::shared_ptr<IUniform> make_uniform(glm::uvec3 value);
std::shared_ptr<IUniform> make_uniform(glm::uvec4 value);

std::shared_ptr<IUniform> make_uniform(glm::mat2 value);
std::shared_ptr<IUniform> make_uniform(glm::mat3 value);
std::shared_ptr<IUniform> make_uniform(glm::mat4 value);

std::shared_ptr<IUniform> make_uniform(glm::mat2x3 value);
std::shared_ptr<IUniform> make_uniform(glm::mat3x2 value);
std::shared_ptr<IUniform> make_uniform(glm::mat2x4 value);
std::shared_ptr<IUniform> make_uniform(glm::mat4x2 value);
std::shared_ptr<IUniform> make_uniform(glm::mat3x4 value);
std::shared_ptr<IUniform> make_uniform(glm::mat4x3 value);

} // glpg
