#ifndef SHADERS_GEOMETRY_SHADER_H
#define SHADERS_GEOMETRY_SHADER_H

// Includes from this project
#include <shaders/abstract_shader.hpp>

// Includes from third party libs
#include <GL/glew.h>

#include <string>

class GeometryShader : public AbstractShader
{
public:
	GeometryShader(std::string const& path)
		: AbstractShader(path)
	{
	}
	
	GLenum get_shader_type() const override
	{
		return GL_GEOMETRY_SHADER;
	}
};

#endif // SHADERS_GEOMETRY_SHADER_H
