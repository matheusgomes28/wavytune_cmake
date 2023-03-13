#ifndef SHADER_SHADER_PROGRAM_H
#define SHADER_SHADER_PROGRAM_H

#include "shaders/shader.h"

// Includes from the STD
#include <memory>
#include <optional>
#include <string>

// Includes from third party
#include <glm/glm.hpp>
#include <glad/glad.h>

struct CompiledShader
{
	GLuint address;
};

struct ShaderProgram
{
	CompiledShader vertex_shader;
	CompiledShader fragment_shader;
	CompiledShader geometry_shader;
	GLuint address;
};

std::optional<ShaderProgram> create_shader_program(ShaderData const& vertex_shader,
  ShaderData const& fragment_shader);

bool set_uniform(ShaderProgram const& program,
  const std::string& name,
  const int& value);

bool set_uniform(ShaderProgram const& program,
  const std::string& name,
  const unsigned int& value);

bool set_uniform(ShaderProgram const& program,
  const std::string& name,
  const float& value);

bool set_uniform(ShaderProgram const& program,
  const std::string& name,
  const glm::vec2& value);

bool set_uniform(ShaderProgram const& program,
  const std::string& name,
  const glm::vec3& value);

bool set_uniform(ShaderProgram const& program,
  const std::string& name,
  const glm::mat4& value);

bool compile_and_link(ShaderProgram const& shader_program);

bool use_shader_program(ShaderProgram const& shader_program);

bool unuse_shader_program(ShaderProgram const& shader_program);

#endif // SHADER_SHADER_PROGRAM_H
