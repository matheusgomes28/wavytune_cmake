// Includes from this project
#include <vector>

#include <glm/glm.hpp>
#include <graphics/draw_buffer.hpp>

DrawBuffer::DrawBuffer() {}

DrawBuffer::DrawBuffer(const std::string& desc) : description_{desc} {}

DrawBuffer::~DrawBuffer() {
    // Dont need anything as the unique pointers
    // will be deleted
}

const std::vector<glm::vec3>& DrawBuffer::get_vertices() const {
    return vertices_;
}

const std::vector<glm::vec3>& DrawBuffer::get_normals() const {
    return normals_;
}

const std::vector<glm::vec2>& DrawBuffer::get_texels() const {
    return texels_;
}

const std::vector<std::uint32_t>& DrawBuffer::get_indices() const {
    return indices_;
}

const std::vector<glm::vec4>& DrawBuffer::get_colours() const {
    return colours_;
}

void DrawBuffer::set_vertices(std::vector<glm::vec3> const& vertices) {
    vertices_ = std::move(vertices);
}

void DrawBuffer::set_normals(std::vector<glm::vec3> const& normals) {
    normals_ = std::move(normals);
}

void DrawBuffer::set_texels(std::vector<glm::vec2> const& texels) {
    texels_ = std::move(texels);
}

void DrawBuffer::set_indices(std::vector<std::uint32_t> const& indices) {
    indices_ = std::move(indices);
}

void DrawBuffer::set_colours(std::vector<glm::vec4> const& colours) {
    colours_ = std::move(colours);
}

std::string DrawBuffer::get_description() const {
    return description_;
}
void DrawBuffer::set_description(const std::string& desc) {
    description_ = desc;
}
