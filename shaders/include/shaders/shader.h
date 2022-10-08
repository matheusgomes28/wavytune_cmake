#ifndef SHADERS_ABSRACT_SHADER_H
#define SHADERS_ABSRACT_SHADER_H

// Includes from the project
#include "data_structures/bytes.h"

// Includes from the STD
#include <string>
#include <stdexcept>

enum class ShaderType
{
	VERTEX,
	FRAGMENT,
	GEOMETRY
};

struct Shader {
	ShaderType type;
	std::vector<char> data;
};

#endif //SHADERS_ABSRACT_SHADER_H
