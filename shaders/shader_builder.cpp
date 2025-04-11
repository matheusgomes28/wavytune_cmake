#include <fstream>
#include <iostream>
#include <iterator>
#include <optional>
#include <shaders/shader.h>
#include <shaders/shader_builder.h>
#include <shaders/shader_program.h>
#include <string>
#include <unistd.h>

namespace {

    std::optional<std::vector<char>> read_file_contents(std::string const& path) {

        std::fstream file{path};
        if (!file) {
            return std::nullopt;
        }

        std::string const shader_str{std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{}};

        return std::make_optional<std::vector<char>>(begin(shader_str), end(shader_str));
    }

} // namespace

ShaderBuilder::ShaderBuilder() {}

ShaderBuilder::~ShaderBuilder() {}

ShaderBuilder& ShaderBuilder::set_fragment_shader(std::string const& path) {

    auto const maybe_data = read_file_contents(path);
    if (!maybe_data) {
        throw ShaderBuilderException();
    }

    fragment_shader_ =
        std::make_optional<ShaderData>(ShaderType::FRAGMENT, std::vector<char>{begin(*maybe_data), end(*maybe_data)});

    return *this;
}

// Geometry shader is optional
ShaderBuilder& ShaderBuilder::set_geometry_shader(std::string const& path) {
    auto const maybe_data = read_file_contents(path);
    if (!maybe_data) {
        std::cout << "invalid geometry shader, skipping it!\n";
        return *this;
    }

    geometry_shader_ =
        std::make_optional<ShaderData>(ShaderType::GEOMETRY, std::vector<char>{begin(*maybe_data), end(*maybe_data)});

    return *this;
}

ShaderBuilder& ShaderBuilder::set_vertex_shader(std::string const& path) {

    auto const maybe_data = read_file_contents(path);
    if (!maybe_data) {
        throw ShaderBuilderException();
    }

    vertex_shader_ =
        std::make_optional<ShaderData>(ShaderType::VERTEX, std::vector<char>{begin(*maybe_data), end(*maybe_data)});

    return *this;
}

std::optional<ShaderProgram> ShaderBuilder::build() {

    if (!vertex_shader_) {
        throw ShaderBuilderException();
    }
    if (!fragment_shader_) {
        throw ShaderBuilderException();
    }

    if (!geometry_shader_) {
        return create_shader_program(*vertex_shader_, *fragment_shader_);
    }

    return create_shader_program(*vertex_shader_, *geometry_shader_, *fragment_shader_);
}
