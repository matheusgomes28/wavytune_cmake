#ifndef SHADERS_VERTEX_SHADER_H
#define SHADERS_VERTEX_SHADER_H

// Includes from this project
#include "abstract_shader.h"

// Includes from third party
#include <GL/glew.h>

class VertexShader : public AbstractShader
{
public:
	VertexShader(const wt::ByteArray<500>& b) 
		: AbstractShader(b)
	{
	}

	VertexShader(wt::ByteArray<500>&& b)
		: AbstractShader(std::forward<wt::ByteArray<500>>(b))
	{
	}

private:
	int _get_shader_type() const override
	{
		return GL_VERTEX_SHADER;
	}
};
#endif // SHADERS_VERTEX_SHADER_H
