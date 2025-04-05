#ifndef SHADERS_FRAGMENT_SHADER_H
#define SHADERS_FRAGMENT_SHADER_H
// Includes from this project
#include "shaders/abstract_shader.hpp"

// Includes from third party
#include <GL/glew.h>

#include <string>

class FragmentShader : public AbstractShader
{
public:
	
	FragmentShader(std::string const& path)
		: AbstractShader(path)
	{
	}

	GLenum get_shader_type() const override
	{
		return GL_FRAGMENT_SHADER;
	}
};
#endif
