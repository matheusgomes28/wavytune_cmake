#ifndef SHADERS_ABSRACT_SHADER_H
#define SHADERS_ABSRACT_SHADER_H

#include <GL/glew.h>

#include <vector>

enum class ShaderType
{
	VERTEX,
	FRAGMENT,
	GEOMETRY
};

struct ShaderData {
	ShaderType type;
	std::vector<char> glsl;
};

#endif //SHADERS_ABSRACT_SHADER_H
