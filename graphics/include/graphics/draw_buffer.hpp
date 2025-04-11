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


    const std::vector<glm::vec3>& get_vertices() const;
    const std::vector<glm::vec3>& get_normals() const;
    const std::vector<glm::vec2>& get_texels() const;
    const std::vector<glm::vec4>& get_colours() const;
    const std::vector<std::uint32_t>& get_indices() const;

    void set_vertices(std::vector<glm::vec3> const& vertices);
    void set_normals(std::vector<glm::vec3> const& normals);
    void set_texels(std::vector<glm::vec2> const& texels);
    void set_colours(std::vector<glm::vec4> const& colours);
    void set_indices(std::vector<std::uint32_t> const& indices);

    std::string get_description() const;
    void set_description(const std::string& texels);

private:
    std::vector<glm::vec3> vertices_;
    std::vector<glm::vec3> normals_;
    std::vector<glm::vec2> texels_;
    std::vector<glm::vec4> colours_;
    std::vector<std::uint32_t> indices_;

    std::string description_;
};

#endif // GRAPHICS_ABSTRACT_DRAW_BUFFER_H
