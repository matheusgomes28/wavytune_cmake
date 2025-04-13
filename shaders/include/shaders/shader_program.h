#ifndef SHADER_SHADER_PROGRAM_H
#define SHADER_SHADER_PROGRAM_H

#include <shaders/shader.h>

// Includes from third party
#include <GL/glew.h>

#include <glm/glm.hpp>

// Includes from the STD
#include <memory>
#include <string>
#include <span>

struct CompiledShader {
    GLuint address;
};

struct ShaderProgram {
    CompiledShader vertex_shader;
    CompiledShader fragment_shader;
    CompiledShader geometry_shader;
    GLuint address;

    bool set_uniform(const std::string& name, const int& value);

    bool set_uniform(const std::string& name, const unsigned int& value);

    bool set_uniform(const std::string& name, const float& value);

    bool set_uniform(const std::string& name, const double& value);

    bool set_uniform(const std::string& name, const glm::vec2& value);

    bool set_uniform(const std::string& name, const glm::vec3& value);

    bool set_uniform(const std::string& name, const glm::mat4& value);

    bool set_uniform(const std::string& name, std::span<float const> values);

    bool set_uniform(const std::string& name, std::span<glm::vec3 const> values);

    bool use();
    
    bool unuse();
};

std::unique_ptr<ShaderProgram> create_shader_program(
    ShaderData const& vertex_shader, ShaderData const& fragment_shader);

std::unique_ptr<ShaderProgram> create_shader_program(
    ShaderData const& vertex_shader, ShaderData const& geometry_shader, ShaderData const& fragment_shader);

#endif // SHADER_SHADER_PROGRAM_H
