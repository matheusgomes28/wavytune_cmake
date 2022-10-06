#ifndef SHADER_SHADER_PROGRAM_H
#define SHADER_SHADER_PROGRAM_H

// Includes from the STD
#include <memory>
#include <string>

// Includes from third party
#include <glm/glm.hpp>
#include <GL/glew.h>

class AbstractShader;
class VertexShader;
class FragmentShader;
class GeometryShader;
class ShaderProgram
{

public:
	
	~ShaderProgram();

	// Setting the stuff
	void set_vertex_shader(std::unique_ptr<VertexShader> vs);
	void set_geometry_shader(std::unique_ptr<GeometryShader> gs);
	void set_fragment_shader(std::unique_ptr<FragmentShader> fs);


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
	std::unique_ptr<VertexShader> vs_;
	std::unique_ptr<GeometryShader> gs_;
	std::unique_ptr<FragmentShader> fs_;

	// Gl related stuff
	GLuint address_ = 0;

	GLuint _generate_address() const;
	void _compile_if_necessary(AbstractShader& s);
	void _attach_shaders();
	void _link_shaders();
	void _detatch_shaders();
};
#endif // SHADER_SHADER_PROGRAM_H
