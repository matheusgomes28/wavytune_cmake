#ifndef GRAPHICS_ABSTRACT_DRAW_BUFFER_H
#define GRAPHICS_ABSTRACT_DRAW_BUFFER_H
#include <glm/fwd.hpp>
#include <glm/glm.hpp>

#include <cstdint>
#include <vector>
#include <string>

class DrawBuffer {
public:
    DrawBuffer();
    DrawBuffer(const std::string& desc);
    virtual ~DrawBuffer();


    std::vector<glm::vec3>& get_vertices();
    std::vector<glm::vec3>& get_normals();
    std::vector<glm::vec2>& get_texels();
    std::vector<glm::vec4>& get_colours();
    std::vector<std::uint32_t>& get_indices();

    void set_vertices(std::vector<glm::vec3> const& vertices);
    void set_vertices(std::vector<glm::vec3>&& vertices);
    void set_normals(std::vector<glm::vec3> const& normals);
    void set_normals(std::vector<glm::vec3>&& normals);
    void set_texels(std::vector<glm::vec2> const& texels);
    void set_texels(std::vector<glm::vec2>&& texels);
    void set_colors(std::vector<glm::vec4> const& colours);
    void set_colors(std::vector<glm::vec4>&& colours);
    void set_indices(std::vector<std::uint32_t> const& indices);
    void set_indices(std::vector<std::uint32_t>&& indices);

    std::string get_description() const;
    void set_description(const std::string& texels);

private:
    std::vector<glm::vec3> vertices_;
    std::vector<glm::vec3> normals_;
    std::vector<glm::vec2> texels_;
    std::vector<glm::vec4> colors_;
    std::vector<std::uint32_t> indices_;

    std::string description_;
};


class DrawBufferBuilder {
public:

    DrawBufferBuilder& add_vertices(std::vector<glm::vec3> const& vertices) {
        vertices_ = vertices;
        return *this;
    }

    DrawBufferBuilder& add_vertices(std::vector<glm::vec3>&& vertices) {
        vertices_ = std::move(vertices);
        return *this;
    }

    DrawBufferBuilder& add_normals(std::vector<glm::vec3> const& normals) {
        normals_ = normals;
        return *this;
    }

    DrawBufferBuilder& add_normals(std::vector<glm::vec3>&& normals) {
        normals_ = std::move(normals);
        return *this;
    }

    DrawBufferBuilder& add_colors(std::vector<glm::vec4> const& colors) {
        colors_ = colors;
        return *this;
    }

    DrawBufferBuilder& add_colors(std::vector<glm::vec4>&& colors) {
        colors_ = std::move(colors);
        return *this;
    }
    
    DrawBufferBuilder& add_indices(std::vector<std::uint32_t> const& indices) {
        indices_ = indices;
        return *this;
    }

    DrawBufferBuilder& add_indices(std::vector<std::uint32_t>&& indices) {
        indices_ = std::move(indices);
        return *this;
    }

    DrawBuffer build() {
        DrawBuffer buffer;
        buffer.set_vertices(std::move(vertices_));
        buffer.set_normals(std::move(normals_));
        buffer.set_colors(std::move(colors_));
        buffer.set_indices(std::move(indices_));
        
        vertices_ = {};
        normals_ = {};
        colors_ = {};
        indices_ = {};

        return buffer;
    }

private:
    std::vector<glm::vec3> vertices_;
    std::vector<glm::vec3> normals_;
    std::vector<glm::vec4> colors_;
    std::vector<std::uint32_t> indices_;
};

#endif // GRAPHICS_ABSTRACT_DRAW_BUFFER_H
