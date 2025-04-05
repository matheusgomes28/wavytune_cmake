#ifndef SHADERS_SHADER_BUILDER_H
#define SHADERS_SHADER_BUILDER_H


// Includes from Shaders
#include <shaders/shader.h>

// Includes from the std
#include <optional>
#include <stdexcept>
#include <string>

struct ShaderProgram;

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
	~ShaderBuilder();

	ShaderBuilder& set_vertex_shader(std::string const& path);
	ShaderBuilder& set_fragment_shader(std::string const& path);
	ShaderBuilder& set_geometry_shader(std::string const& path);

	std::optional<ShaderProgram> build();

private:
	std::optional<ShaderData> vertex_shader_;
	std::optional<ShaderData> geometry_shader_;
	std::optional<ShaderData> fragment_shader_;
};

#endif // SHADERS_SHADER_BUILDER_H
