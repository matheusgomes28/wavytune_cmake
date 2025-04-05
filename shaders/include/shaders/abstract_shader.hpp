#ifndef SHADERS_ABSTRACT_SHADER_H
#define SHADERS_ABSTRACT_SHADER_H

#include "shaders/shader.h"
#include "shaders/shader_program.h"
#include <string>
#include <GL/glew.h>

// TODO : Make sure this is all done correctly with inheritance
// and abstraction.
class TextReader;
class AbstractShader
{
	public:
		AbstractShader(ShaderData const& shader_data);

		bool compile();
		unsigned int get_address() const;
		std::string get_compilation_message() const;

        virtual GLenum get_shader_type() const;


	private:
		ShaderData data_;
		unsigned int address_;

		unsigned int generateBuffer() const;
		std::string loadShaderData() const;
		virtual int getGlEnum() const = 0;

};

#endif // SHADERS_ABSTRACT_SHADER_H
