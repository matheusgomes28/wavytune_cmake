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

std::vector<glm::vec3>& DrawBuffer::get_vertices() {
    return vertices_;
}

std::vector<glm::vec3>& DrawBuffer::get_normals() {
    return normals_;
}

std::vector<glm::vec2>& DrawBuffer::get_texels() {
    return texels_;
}

std::vector<std::uint32_t>& DrawBuffer::get_indices() {
    return indices_;
}

std::vector<glm::vec4>& DrawBuffer::get_colours() {
    return colors_;
}

void DrawBuffer::set_vertices(std::vector<glm::vec3> const& vertices) {
    vertices_ = vertices;
}

void DrawBuffer::set_vertices(std::vector<glm::vec3>&& vertices) {
    vertices_ = std::move(vertices);
}

void DrawBuffer::set_normals(std::vector<glm::vec3> const& normals) {
    normals_ = normals;
}

void DrawBuffer::set_normals(std::vector<glm::vec3>&& normals) {
    normals_ = std::move(normals);
}

void DrawBuffer::set_texels(std::vector<glm::vec2> const& texels) {
    texels_ = texels;
}

void DrawBuffer::set_texels(std::vector<glm::vec2>&& texels) {
    texels_ = std::move(texels);
}

void DrawBuffer::set_indices(std::vector<std::uint32_t> const& indices) {
    indices_ = indices;
}

void DrawBuffer::set_indices(std::vector<std::uint32_t>&& indices) {
    indices_ = std::move(indices);
}

void DrawBuffer::set_colors(std::vector<glm::vec4> const& colors) {
    colors_ = colors;
}

void DrawBuffer::set_colors(std::vector<glm::vec4>&& colors) {
    colors_ = std::move(colors);
}

std::string DrawBuffer::get_description() const {
    return description_;
}
void DrawBuffer::set_description(const std::string& desc) {
    description_ = desc;
}
