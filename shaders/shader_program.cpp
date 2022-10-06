// Includes from this project
#include "shaders/shader_program.h"
#include "shaders/vertex_shader.h"
#include "shaders/fragment_shader.h"
#include "shaders/geometry_shader.h"

// Includes from the STD
#include <exception>
#include <iostream>

// Includes from third party
#include <glm/gtc/type_ptr.hpp>

void ShaderProgram::set_fragment_shader(std::unique_ptr<FragmentShader> fs)
{
	fs_ = std::move(fs);
}

void ShaderProgram::set_geometry_shader(std::unique_ptr<GeometryShader> gs)
{
	gs_ = std::move(gs);
}

void ShaderProgram::set_vertex_shader(std::unique_ptr<VertexShader> vs)
{
	vs_ = std::move(vs);
}

GLuint ShaderProgram::_generate_address() const
{
	return glCreateProgram();
}

void ShaderProgram::compile_and_link()
{
	// Generate the address for this program
	address_ = _generate_address();

	// Compile all the shaders, generating the
	// OpenGL address_

	_compile_if_necessary(*vs_);
	if (gs_) _compile_if_necessary(*gs_);
	_compile_if_necessary(*fs_);

	std::cout << "Vertex Shader: " << vs_->get_compilation_message() << std::endl;
	std::cout << "Fragment Shader: " << fs_->get_compilation_message() << std::endl;
	if (gs_) std::cout << "Geometry Shader: " << gs_->get_compilation_message() << std::endl;

	// Link all the shaders
	_attach_shaders();
	_link_shaders();
	_detatch_shaders();
}

void ShaderProgram::_compile_if_necessary(AbstractShader& s)
{
	if (s.get_address() == 0)
	{
		s.compile();
	}
}

void ShaderProgram::use()
{
	if (address_ != 0) {
		glUseProgram(address_);
	}
	else {
		throw std::logic_error("Cannot use uncompiled shader program.");
	}
}

void ShaderProgram::unuse()
{
}

void ShaderProgram::_attach_shaders()
{
	if (address_ > 0)
	{
		glAttachShader(address_, vs_->get_address());
		if (gs_) {
			glAttachShader(address_, gs_->get_address());
		}
		glAttachShader(address_, fs_->get_address());
	}
	else
	{
		throw std::logic_error("Address not created created for shader program");
	}
}

void ShaderProgram::_link_shaders()
{
	if (address_ > 0) {
		// Need to attach all the addresses
		glLinkProgram(address_);
	}
	else {
		throw std::logic_error("Address not created created for shader program");
	}
}

void ShaderProgram::_detatch_shaders()
{
	if (address_ > 0)
	{
		glDetachShader(address_, vs_->get_address());
		if (gs_)
		{
			glDetachShader(address_, gs_->get_address());
		}
		glDetachShader(address_, fs_->get_address());
	}
	else
	{
		throw std::logic_error("Address not created created for shader program");
	}
}

GLuint ShaderProgram::get_address() const
{
	return address_;
}

void ShaderProgram::set_uniform(const std::string& name, const int& value) const
{
	if (address_ > 0) {
		// Get the location of the uniform then set it
		int uniformLoc = glGetUniformLocation(address_, name.c_str());
		glUniform1i(uniformLoc, value);
	}
}

void ShaderProgram::set_uniform(const std::string& name, const unsigned int& value) const
{
	if (address_ > 0) {
		// Get the location of the uniform then set it
		int uniformLoc = glGetUniformLocation(address_, name.c_str());
		glUniform1ui(uniformLoc, value);
	}
}

void ShaderProgram::set_uniform(const std::string& name, const float& value) const
{
	if (address_ > 0) {
		int uniformLoc = glGetUniformLocation(address_, name.c_str());
		glUniform1f(uniformLoc, value);
	}
}

void ShaderProgram::set_uniform(const std::string& name, const double& value) const
{
	if (address_ > 0) {
		int uniformLoc = glGetUniformLocation(address_, name.c_str());
		glUniform1d(uniformLoc, value);
	}
}

void ShaderProgram::set_uniform(const std::string& name, const glm::vec2& value) const
{
	if (address_ > 0) {
		int uniformLoc = glGetUniformLocation(address_, name.c_str());
		glUniform2f(uniformLoc, value.x, value.y);
	}
}

void ShaderProgram::set_uniform(const std::string& name, const glm::vec3& value) const
{
	if (address_ > 0) {
		int uniformLoc = glGetUniformLocation(address_, name.c_str());
		glUniform3f(uniformLoc, value.x, value.y, value.z);
	}
}

void ShaderProgram::set_uniform(const std::string& name, const glm::mat4& value) const
{
	if (address_ > 0) {
		GLint uniformLoc = glGetUniformLocation(address_, name.c_str());
		glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(value));
	}
}


ShaderProgram::~ShaderProgram()
{
	// Delete if the address exists
	if (address_ > 0) {
		glDeleteProgram(address_);
	}
}
