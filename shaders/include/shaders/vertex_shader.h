#ifndef SHADERS_VERTEX_SHADER_H
#define SHADERS_VERTEX_SHADER_H

#include <shaders/abstract_shader.hpp>

#include <GL/glew.h>

#include <string>

class VertexShader : public AbstractShader
{
public:
	VertexShader(std::string const& path)
		: AbstractShader(path)
	{
	}

private:
	GLenum get_shader_type() const override
	{
		return GL_VERTEX_SHADER;
	}
};
#endif // SHADERS_VERTEX_SHADER_H
