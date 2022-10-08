#ifndef SHADER_SHADER_PROGRAM_H
#define SHADER_SHADER_PROGRAM_H

// Includes from the STD
#include <memory>
#include <string>

// Includes from third party
#include <glm/glm.hpp>
#include <GL/glew.h>

struct Shader;

struct ShaderProgram
{
	std::unique_ptr<Shader> vertex_shader;
	std::unique_ptr<Shader> fragment_shader;
	std::unique_ptr<Shader> geometry_shader;
	GLuint address;
};

void set_uniform(ShaderProgram const& program, const std::string& name, const int& value);
void set_uniform(ShaderProgram const& program, const std::string& name, const unsigned int& value);
void set_uniform(ShaderProgram const& program, const std::string& name, const float& value);
void set_uniform(ShaderProgram const& program, const std::string& name, const double& value);
void set_uniform(ShaderProgram const& program, const std::string& name, const glm::vec2& value);
void set_uniform(ShaderProgram const& program, const std::string& name, const glm::vec3& value);
void set_uniform(ShaderProgram const& program, const std::string& name, const glm::mat4& value);

void compile_and_link(ShaderProgram const&);
void use();
void unuse();

GLuint get_address() const;


class ShaderProgram
{

public:
	
	~ShaderProgram();

	// Uniform setting stuff
	void set_uniform(const std::string& name, const int& value) const;
	void set_uniform(const std::string& name, const unsigned int& value) const;
	void set_uniform(const std::string& name, const float& value) const;
	void set_uniform(const std::string& name, const double& value) const;
	void set_uniform(const std::string& name, const glm::vec2& value) const;
	void set_uniform(const std::string& name, const glm::vec3& value) const;
	void set_uniform(const std::string& name, const glm::mat4& value) const;

	void compile_and_link();
	void use();
	void unuse();

	GLuint get_address() const;

private:

	// The shaders we have
	std::unique_ptr<Shader> vs_;
	std::unique_ptr<Shader> gs_;
	std::unique_ptr<Shader> fs_;

	// Gl related stuff
	GLuint address_ = 0;

	GLuint _generate_address() const;
	void _compile_if_necessary(AbstractShader& s);
	void _attach_shaders();
	void _link_shaders();
	void _detatch_shaders();
};
#endif // SHADER_SHADER_PROGRAM_H
