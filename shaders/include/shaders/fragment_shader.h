#ifndef SHADERS_FRAGMENT_SHADER_H
#define SHADERS_FRAGMENT_SHADER_H
// Includes from this project
#include "abstract_shader.h"

// Includes from third party
#include <gl/glew.h>


class FragmentShader : public AbstractShader
{
public:
	
	FragmentShader(const wt::ByteArray<500>& b)
		: AbstractShader(b)
	{
	}

	FragmentShader(wt::ByteArray<500>&& b)
		: AbstractShader(std::forward<wt::ByteArray<500>>(b))
	{
	}

private:
	int _get_shader_type() const override
	{
		return GL_FRAGMENT_SHADER;
	}
};
#endif
