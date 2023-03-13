// Includes from this project
#include "shaders/shader_program.h"
#include "shaders/shader.h"

// Includes from the STD
#include <exception>
#include <iostream>
#include <optional>

// Includes from third party
#include <glm/gtc/type_ptr.hpp>

namespace
{
	GLuint generate_address()
	{
		return glCreateProgram();
	}
	
	std::optional<CompiledShader> compile_shader(ShaderData const& shader)
	{
		auto const shader_type = [&shader](){
			switch (shader.type)
			{
				case ShaderType::VERTEX:
					return GL_VERTEX_SHADER;
				case ShaderType::FRAGMENT:
					return GL_FRAGMENT_SHADER;
				case ShaderType::GEOMETRY:
					return GL_GEOMETRY_SHADER;
				default:
					return GL_VERTEX_SHADER;
			}
		}();
		
		auto const address = glCreateShader(shader_type);
		if (address != 0)
		{
			auto str_data = shader.glsl.data();
			int data_size = shader.glsl.size();
			glShaderSource(address, 1, &str_data, &data_size);
			glCompileShader(address);

			GLint compiled = GL_FALSE;
			glGetShaderiv(address, GL_COMPILE_STATUS, &compiled);
			if (compiled != GL_TRUE)
			{
				return std::nullopt;
			}
		}

		return CompiledShader{address};
	}

	bool attach_shaders(ShaderProgram const& shader_program)
	{
		auto const& address = shader_program.address;
		if (address > 0)
		{
			// TODO : check for attach shader errors
			//        read docs for possible GL errors
			glAttachShader(address, shader_program.vertex_shader.address);
			if (shader_program.geometry_shader.address > 0) {
				glAttachShader(address, shader_program.geometry_shader.address);
			}
			glAttachShader(address, shader_program.geometry_shader.address);
			return true;
		}

		return false;
	}

	bool link_shaders(ShaderProgram const& shader_program)
	{
		if (shader_program.address > 0) {
			// Need to attach all the addresses
			// TODO : find possible OpenGLerrors for linking
			//        the shaders here
			glLinkProgram(shader_program.address);
			return true;
		}

		return false;
	}

	bool detach_shaders(ShaderProgram const& shader_program)
	{
		// TODO : do I need to check all of these
		//        shader addresses???
		const auto address = shader_program.address;
		if (address > 0)
		{
			glDetachShader(address, shader_program.vertex_shader.address);
			if (shader_program.geometry_shader.address > 0)
			{
				glDetachShader(address, shader_program.geometry_shader.address);
			}
			glDetachShader(address, shader_program.fragment_shader.address);
			return true;
		}

		return false;
	}
} // namespace


// ShaderProgram needs the shaders to be compiled
// at the moment!
bool compile_and_link(ShaderProgram const& shader_program)
{

	auto& vertex_shader = shader_program.vertex_shader;
	if (vertex_shader.address <= 0) // are opengl addresses always > 0??
	{
		return false;
	}
	
	auto& fragment_shader = shader_program.fragment_shader;
	if (fragment_shader.address <= 0)
	{
		return false;
	}

	auto& geometry_shader = shader_program.geometry_shader;
	if (geometry_shader.address <= 0)
	{
		return false;
	}

	auto const address = generate_address();

	// TODO : print some stupid error messages on failures
	if (!attach_shaders(shader_program))
	{
		return false;
	}
	
	if (!link_shaders(shader_program))
	{
		return false;
	}

	if (!detach_shaders(shader_program))
	{
		return false;
	}

	return true;
}

/*
void ShaderProgram::_compile_if_necessary(AbstractShader& s)
{
	if (s.get_address() == 0)
	{
		s.compile();
	}
}
*/

bool use_shader_program(ShaderProgram const& program)
{
	if (program.address != 0) {
		glUseProgram(program.address);
	}

	return false;
}

bool unuse_shader_program(ShaderProgram const& program)
{
	glUseProgram(0);
	return true;
}

bool set_uniform(ShaderProgram const& shader_program,
  const std::string& name,
  const int& value)
{
	const auto address = shader_program.address;
	if (address > 0) {
		// Get the location of the uniform then set it
		int uniformLoc = glGetUniformLocation(address, name.c_str());
		glUniform1i(uniformLoc, value);
		
		// TODO : check for errors before we return true
		return true;
	}

	return false;
}

std::optional<ShaderProgram> create_shader_program(ShaderData const& vertex_shader_glsl,
  ShaderData const& geometry_shader_glsl,
  ShaderData const& fragment_shader_glsl)
{
	// TODO : display the errors in logs
	auto const vertex_shader = compile_shader(vertex_shader_glsl);
	if (!vertex_shader)
	{
		return std::nullopt;
	}

	auto const geometry_shader = compile_shader(geometry_shader_glsl);
	if (!geometry_shader)
	{
		return std::nullopt;
	}

	auto const fragment_shader = compile_shader(fragment_shader_glsl);
	if (!fragment_shader)
	{
		return std::nullopt;
	}

	// Create address for shader program
	auto const address = glCreateProgram();
	if (address == 0)
	{
		return std::nullopt;
	}

	return ShaderProgram{*vertex_shader, *fragment_shader, *geometry_shader, address};
}


bool set_uniform(ShaderProgram const& shader_program,
  const std::string& name,
  const unsigned int& value)
{
	const auto address = shader_program.address;
	if (address > 0) {
		// Get the location of the uniform then set it
		int uniformLoc = glGetUniformLocation(address, name.c_str());
		glUniform1ui(uniformLoc, value);
		// TODO : check for errors before we return true
		return true;
	}

	return false;
}

bool set_uniform(ShaderProgram const& shader_program,
  const std::string& name,
  const float& value)
{
	const auto address = shader_program.address;
	if (address > 0) {
		int uniformLoc = glGetUniformLocation(address, name.c_str());
		glUniform1f(uniformLoc, value);
		// TODO : check for errors before we return true
		return true;
	}

	return true;
}

bool set_uniform(ShaderProgram const& shader_program,
  const std::string& name,
  const double& value)
{
	const auto address = shader_program.address;
	if (address > 0) {
		int uniformLoc = glGetUniformLocation(address, name.c_str());
		// glUniform1d(uniformLoc, value);
		// TODO : check for errors before we return true
		return true;
	}

	return false;
}

bool set_uniform(ShaderProgram const& shader_program,
  const std::string& name,
  const glm::vec2& value)
{
	const auto address = shader_program.address;
	if (address > 0) {
		int uniformLoc = glGetUniformLocation(address, name.c_str());
		glUniform2f(uniformLoc, value.x, value.y);
		// TODO : check for errors before we return true
		return true;
	}

	return false;
}

bool set_uniform(ShaderProgram const& shader_program,
  std::string const& name,
  glm::vec3 const& value)
{
	const auto address = shader_program.address;
	if (address > 0) {
		int uniformLoc = glGetUniformLocation(address, name.c_str());
		glUniform3f(uniformLoc, value.x, value.y, value.z);
		// TODO : check for errors before we return true
		return true;
	}

	return false;
}

bool set_uniform(ShaderProgram const& shader_program,
  const std::string& name,
  const glm::mat4& value)
{
	const auto address = shader_program.address;
	if (address > 0) {
		GLint uniformLoc = glGetUniformLocation(address, name.c_str());
		glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(value));
		// TODO : check for errors before we return true
		return true;
	}

	return false;
}

/*
ShaderProgram::~ShaderProgram()
{
	// Delete if the address exists
	if (address_ > 0) {
		glDeleteProgram(address_);
	}
}
*/
