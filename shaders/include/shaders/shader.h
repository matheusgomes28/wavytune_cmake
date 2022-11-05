#ifndef SHADERS_ABSRACT_SHADER_H
#define SHADERS_ABSRACT_SHADER_H

// Includes from the project
#include "data_structures/bytes.h"

// Includes from the STD
#include <string>
#include <stdexcept>

#include <GL/glew.h>

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
