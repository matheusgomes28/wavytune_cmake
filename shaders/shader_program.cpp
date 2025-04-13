// Includes from this project
#include "shaders/shader_program.h"

#include "shaders/shader.h"

// Includes from the STD
#include <memory>
#include <optional>
#include <span>

// Includes from third party
#include <GL/glew.h>
#include <gsl/assert>

#include <glm/gtc/type_ptr.hpp>

namespace {
    std::optional<CompiledShader> compile_shader(ShaderData const& shader) {
        auto const shader_type = [&shader]() {
            switch (shader.type) {
            case ShaderType::VERTEX:
                return GL_VERTEX_SHADER;
            case ShaderType::FRAGMENT:
                return GL_FRAGMENT_SHADER;
            case ShaderType::GEOMETRY:
                return GL_GEOMETRY_SHADER;
            }
        }();

        auto const address = glCreateShader(shader_type);
        if (address != 0) {
            auto str_data = shader.glsl.data();
            int data_size = shader.glsl.size();
            glShaderSource(address, 1, &str_data, &data_size);
            glCompileShader(address);

            GLint compiled = GL_FALSE;
            glGetShaderiv(address, GL_COMPILE_STATUS, &compiled);
            if (compiled != GL_TRUE) {
                // error
                GLchar InfoLog[256];
                glGetShaderInfoLog(address, sizeof(InfoLog), NULL, InfoLog);
                return std::nullopt;
            }
        }

        return std::make_optional<CompiledShader>(address);
    }

    bool attach_shaders(std::uint32_t program_address, std::uint32_t vertex_address, std::uint32_t geometry_address,
        std::uint32_t fragment_address) {
        Expects(program_address > 0);

        // TODO : check for attach shader errors
        //        read docs for possible GL errors
        glAttachShader(program_address, vertex_address);
        if (geometry_address > 0) {
            glAttachShader(program_address, geometry_address);
        }
        glAttachShader(program_address, fragment_address);

        return true;
    }

    bool link_shaders(std::uint32_t program_address) {
        Expects(program_address > 0);

        // TODO : Need to find a nice way to get errors
        glLinkProgram(program_address);
        return true;
    }

    bool detach_shaders(std::uint32_t program_address, std::uint32_t vertex_address, std::uint32_t geometry_address, std::uint32_t fragment_address) {
        Expects(program_address > 0);
        glDetachShader(program_address, vertex_address);
        if (geometry_address > 0) {
            glDetachShader(program_address, geometry_address);
        }
        glDetachShader(program_address, fragment_address);
        return true;
    }

    // ShaderProgram needs the shaders to be compiled
    // at the moment!
    bool compile_and_link(std::uint32_t program_address, std::uint32_t vertex_address, std::uint32_t geometry_address,
        std::uint32_t fragment_address) {

        if (vertex_address <= 0) // are opengl addresses always > 0??
        {
            return false;
        }

        if (fragment_address <= 0) {
            return false;
        }

        // TODO : print some stupid error messages on failures
        if (!attach_shaders(program_address, vertex_address, geometry_address, fragment_address)) {
            return false;
        }

        if (!link_shaders(program_address)) {
            return false;
        }

        if (!detach_shaders(program_address, vertex_address, geometry_address, fragment_address)) {
            return false;
        }

        return true;
    }
} // namespace


bool ShaderProgram::use() {
    if (address != 0) {
        glUseProgram(address);
    }

    return false;
}

bool ShaderProgram::unuse() {
    glUseProgram(0);
    return true;
}

bool ShaderProgram::set_uniform(const std::string& name, const int& value) {
    Expects(address > 0);

    // Get the location of the uniform then set it
    int uniformLoc = glGetUniformLocation(address, name.c_str());
    glUniform1i(uniformLoc, value);

    // TODO : check for errors before we return true
    return true;
}

bool ShaderProgram::set_uniform(const std::string& name, const unsigned int& value) {
    Expects(address > 0);

    // Get the location of the uniform then set it
    int uniformLoc = glGetUniformLocation(address, name.c_str());
    glUniform1ui(uniformLoc, value);
    // TODO : check for errors before we return true
    return true;
}

bool ShaderProgram::set_uniform(const std::string& name, const float& value) {
    Expects(address > 0);

    int uniformLoc = glGetUniformLocation(address, name.c_str());
    glUniform1f(uniformLoc, value);
    // TODO : check for errors before we return true
    return true;
}

bool ShaderProgram::set_uniform(const std::string& name, const double& value) {
    Expects(address > 0);

    int uniformLoc = glGetUniformLocation(address, name.c_str());
    glUniform1d(uniformLoc, value);
    // TODO : check for errors before we return true
    return true;
}

bool ShaderProgram::set_uniform(const std::string& name, const glm::vec2& value) {
    Expects(address > 0);

    int uniformLoc = glGetUniformLocation(address, name.c_str());
    glUniform2f(uniformLoc, value.x, value.y);
    // TODO : check for errors before we return true
    return true;
}

bool ShaderProgram::set_uniform(std::string const& name, glm::vec3 const& value) {
    Expects(address > 0);

    int uniformLoc = glGetUniformLocation(address, name.c_str());
    glUniform3f(uniformLoc, value.x, value.y, value.z);
    // TODO : check for errors before we return true
    return true;
}

bool ShaderProgram::set_uniform(const std::string& name, const glm::mat4& value) {
    Expects(address > 0);

    GLint uniformLoc = glGetUniformLocation(address, name.c_str());
    glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(value));
    // TODO : check for errors before we return true
    return true;
}

bool ShaderProgram::set_uniform(const std::string& name, std::span<float const> values) {
    Expects(address > 0);
    Expects(values.size() > 0);

    GLint uniformLoc = glGetUniformLocation(address, name.c_str());
    glUniform1fv(uniformLoc, values.size(), &values[0]);

    // TODO : check for errors before we return true
    return true;
}

bool ShaderProgram::set_uniform(const std::string& name, std::span<glm::vec3 const> values) {
    Expects(address > 0);
    Expects(values.size() > 0);

    GLint uniformLoc = glGetUniformLocation(address, name.c_str());
    glUniform3fv(uniformLoc, values.size(), glm::value_ptr(values[0]));

    // TODO : check for errors before we return true
    return true;
}

std::unique_ptr<ShaderProgram> create_shader_program(
    ShaderData const& vertex_shader_glsl, ShaderData const& fragment_shader_glsl) {

    // TODO : display the errors in logs
    auto const vertex_shader = compile_shader(vertex_shader_glsl);
    if (!vertex_shader) {
        return {};
    }

    auto const fragment_shader = compile_shader(fragment_shader_glsl);
    if (!fragment_shader) {
        return {};
    }

    // Create address for shader program
    auto const address = glCreateProgram();
    if (address == 0) {
        return {};
    }

    CompiledShader const geometry_shader{.address = 0};
    compile_and_link(address, vertex_shader->address, geometry_shader.address, fragment_shader->address);
    return std::make_unique<ShaderProgram>(*vertex_shader, *fragment_shader, geometry_shader, address);
}

std::unique_ptr<ShaderProgram> create_shader_program(ShaderData const& vertex_shader_glsl,
    ShaderData const& geometry_shader_glsl, ShaderData const& fragment_shader_glsl) {
    // TODO : display the errors in logs
    auto const vertex_shader = compile_shader(vertex_shader_glsl);
    if (!vertex_shader) {
        return {};
    }

    auto const geometry_shader = compile_shader(geometry_shader_glsl);
    if (!geometry_shader) {
        return {};
    }

    auto const fragment_shader = compile_shader(fragment_shader_glsl);
    if (!fragment_shader) {
        return {};
    }

    // Create address for shader program
    auto const address = glCreateProgram();
    if (address == 0) {
        return {};
    }

    compile_and_link(address, vertex_shader->address, geometry_shader->address, fragment_shader->address);
    return std::make_unique<ShaderProgram>(*vertex_shader, *fragment_shader, *geometry_shader, address);
}
