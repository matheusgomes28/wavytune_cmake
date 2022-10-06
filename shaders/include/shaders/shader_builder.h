#ifndef SHADERS_SHADER_BUILDER_H
#define SHADERS_SHADER_BUILDER_H


// Includes from DataStructures
#include <data_structures/bytes.h>

// Includes from Shaders
#include "fragment_shader.h"
#include "geometry_shader.h"
#include "vertex_shader.h"
#include "shader_program.h"

// Includes from the std
#include <memory>
#include <stdexcept>


class ShaderBuilderException : std::runtime_error
{
public:
	ShaderBuilderException()
		: std::runtime_error("Builder state invalid")
	{
	}
};

class ShaderBuilder
{
public:
	ShaderBuilder();

	ShaderBuilder& set_vertex_shader(const wt::ByteArray<500>& data);
	ShaderBuilder& set_vertex_shader(wt::ByteArray<500>&& data);
	ShaderBuilder& set_vertex_shader(std::unique_ptr<VertexShader> vs);

	ShaderBuilder& set_fragment_shader(const wt::ByteArray<500>& data);
	ShaderBuilder& set_fragment_shader(wt::ByteArray<500>&& data);
	ShaderBuilder& set_fragment_shader(std::unique_ptr<FragmentShader> fs);

	ShaderBuilder& set_geometry_shader(const wt::ByteArray<500>& data);
	ShaderBuilder& set_geometry_shader(wt::ByteArray<500>&& data);
	ShaderBuilder& set_geometry_shader(std::unique_ptr<GeometryShader> vs);

	std::unique_ptr<ShaderProgram> build();

private:
	std::unique_ptr<ShaderProgram> shader_program_;
};

#endif // SHADERS_SHADER_BUILDER_H
