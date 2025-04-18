#define GLM_ENABLE_EXPERIMENTAL 1

// clang-format off
// Must be included before gl
#include <GL/glew.h>
//clang-format on

#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <alloca.h>
#include <gsl/assert>
#include <shaders/shader_program.h>

#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <graphics/concrete_renderer.hpp>
#include <graphics/draw_buffer.hpp>

#include <array>
#include <cmath>
#include <optional>
#include <span>
#include <string>


namespace {

    std::vector<std::string> getError() {
        std::vector<std::string> retVal;
        GLenum error;
        while ((error = glGetError()) != GL_NO_ERROR) {

            switch (error) {
            case GL_INVALID_ENUM:
                retVal.push_back("Invalid enum.");
                break;
            case GL_INVALID_VALUE:
                retVal.push_back("Invalid function value.");
                break;
            case GL_INVALID_OPERATION:
                retVal.push_back("Invalid operation.");
                break;
            case GL_STACK_OVERFLOW:
                retVal.push_back("stack overflow.");
                break;
            case GL_STACK_UNDERFLOW:
                retVal.push_back("stack underflow.");
                break;
            case GL_OUT_OF_MEMORY:
                retVal.push_back("Out of memory.");
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                retVal.push_back("Invalid framebuffer operation.");
                break;
            case GL_TABLE_TOO_LARGE:
                retVal.push_back("Table is too large.");
                break;
            }
        }
        return retVal;
    }

    /**
     * @brief used to set the vertex pointer attributes
     * for a given buffer.
     */
    struct VertexAttribPointerConfig {
        std::uint64_t offset;
        std::uint32_t size;
        std::uint32_t stride;
        std::uint32_t data_type;
        std::uint32_t buffer_type;
        bool normalized;
    };

    void configure_vertex_attrib(std::uint32_t shader_address, std::uint32_t vbo_address, std::string const& attr_name, VertexAttribPointerConfig const& config) {
        Expects(shader_address != 0);
        Expects(vbo_address != 0);

        std::vector<std::string> errors;
        errors = getError();
        glBindBuffer(config.buffer_type, vbo_address);
        errors = getError();

        const std::int32_t pos_ptr = glGetAttribLocation(shader_address, attr_name.c_str());
        errors = getError();
        Expects(pos_ptr >= 0);

        glVertexAttribPointer(
            pos_ptr, config.size, config.data_type, config.normalized, config.stride, (GLvoid*) config.offset);
        errors = getError();

        glBindBuffer(config.buffer_type, 0);
        errors = getError();
    }

    std::optional<std::uint32_t> create_vao() {
        std::uint32_t array_id = 0;
        glGenVertexArrays(1, &array_id);

        if (array_id == 0) {
            return std::nullopt;
        }

        return array_id;
    }

    void delete_vao(std::uint32_t array_id) {
        Expects(array_id != 0);
        glDeleteVertexArrays(1, &array_id);
    }

    std::optional<std::uint32_t> create_vbo() {
        std::uint32_t buffer_id = 0;
        glGenBuffers(1, &buffer_id);

        if (buffer_id == 0) {
            return std::nullopt;
        }

        return buffer_id;
    }

    void delete_vbo(std::uint32_t buffer_id) {
        Expects(buffer_id != 0);
        glDeleteBuffers(1, &buffer_id);
    }

    void allocate_buffer_memory(std::uint32_t vbo, std::uint32_t buffer_type, std::uint32_t size) {
        Expects(vbo != 0);

        // TODO : Do we want to just provide a function that will copy
        // TODO : the data straight away? We're running an allocation THEN
        // TODO : a copy.
        glBindBuffer(buffer_type, vbo);
        glBufferData(buffer_type, size, nullptr, GL_STATIC_DRAW);
        glBindBuffer(buffer_type, 0);
    }

    // TODO : This void* will be complained at, maybe we use a span???
    // TODO : This should probably return an error in case data is larger than buffer
    std::uint32_t add_buffer_data(std::span<std::uint8_t> data, std::uint32_t vbo, std::uint32_t buffer_type, const std::uint32_t offset) {
        Expects(vbo != 0);

        // TODO : These can throw a lot of errors, make sure we catch them
        glBindBuffer(buffer_type, vbo);
        glBufferSubData(buffer_type, offset, data.size(), reinterpret_cast<void*>(data.data()));

        // TODO : This is wrong, we should check if we actually copied everything
        return static_cast<std::uint32_t>(data.size());
    }

    // TODO : Ideally
    template <typename T>
    std::uint32_t add_buffer_data(std::vector<T>& vertices, std::uint32_t vbo, std::uint32_t buffer_type, std::uint32_t offset) {
        std::uint32_t const vertices_byte_size = vertices.size() * sizeof(T);
        std::span<std::uint8_t> const vertices_span{reinterpret_cast<std::uint8_t*>(&vertices[0]), vertices_byte_size};
        return add_buffer_data(vertices_span, vbo, buffer_type, offset);
    }
} // namespace


ConcreteRenderer::ConcreteRenderer(std::unique_ptr<ShaderProgram>&& shader) : vao_{0}, shader_program_{std::move(shader)}, points_to_draw_{0}, vertex_offset_{0}, normal_offset_{0}, color_offset_{0}, index_offset_{0} {
    
    // Debug to make sure our shader is actually valid
    Expects(shader_program_);
    Expects(shader_program_->address != 0);

    auto const vao_address = create_vao();

    if (!vao_address) {
        throw VaoCreationException();
    }
    vao_ = *vao_address;

    std::array<BufferType, 4> constexpr types = {BufferType::Vertex, BufferType::Normal, BufferType::Color, BufferType::Index};
    for (auto const type : types) {

        auto vbo_address = create_vbo();
        if (!vbo_address) {
            throw VboCreationException();
        }

        vbos_.insert({type, *vbo_address});
    }
}

ConcreteRenderer::~ConcreteRenderer() {
    delete_vao(vao_);

    for (auto const& [buffer_type, vbo] : vbos_) {
        delete_vbo(vbo);
    }
}

void ConcreteRenderer::send_gpu_data() {
    shader_program_->use();

    // TODO : Why is this commented out?
    glBindVertexArray(vao_);

    std::vector<std::string> errors;


    // Allocate enough memory at the buffers
    errors = getError();
    _allocate_gpu_memory();
    errors = getError();
    _populate_gpu_buffers();
    errors = getError();
    _set_gpu_vertex_attributes();
    errors = getError();
    _set_gpu_normal_attributes();
    errors = getError();
    _set_gpu_colour_attributes();
    errors = getError();


    // TODO : Do we need this call here?
    glUseProgram(0);
    shader_program_->unuse();
}

// TODO : For now, this assumes that all the
// TODO : render data will have been added before
// TODO : we actually allocate everything, we may
// TODO : want to look at moving or double buffering
// TODO : this buffer to make it expandable
void ConcreteRenderer::_allocate_gpu_memory() {

    // TODO : We may want to configure this size in the future.
    // TODO : for now we allow 2M vertices, normals, colours to
    // TODO : be drawn.
    std::uint32_t constexpr DEFAULT_BUFFER_SIZE = sizeof(float) * 2'000'000;

    auto const vertex_address = vbos_[BufferType::Vertex];
    auto const normal_address = vbos_[BufferType::Normal];
    auto const color_vbo      = vbos_[BufferType::Color];
    auto const index_vbo = vbos_[BufferType::Index];

    allocate_buffer_memory(vertex_address, GL_ARRAY_BUFFER, DEFAULT_BUFFER_SIZE);
    allocate_buffer_memory(normal_address, GL_ARRAY_BUFFER, DEFAULT_BUFFER_SIZE);
    allocate_buffer_memory(color_vbo, GL_ARRAY_BUFFER, DEFAULT_BUFFER_SIZE);
    allocate_buffer_memory(index_vbo, GL_ELEMENT_ARRAY_BUFFER, DEFAULT_BUFFER_SIZE);
}

void ConcreteRenderer::_populate_gpu_buffers() {
    auto const vertex_vbo = vbos_[BufferType::Vertex];
    auto const normal_vbo = vbos_[BufferType::Normal];
    auto const color_vbo  = vbos_[BufferType::Color];
    auto const index_vbo  = vbos_[BufferType::Index];

    while (!pending_buffers_.empty()) {
        auto draw_buffer = std::move(pending_buffers_.front());
        pending_buffers_.pop();

        // vertex data
        auto& vertices = draw_buffer.get_vertices();
        vertex_offset_ += add_buffer_data(vertices, vertex_vbo, GL_ARRAY_BUFFER, vertex_offset_);

        auto& normals = draw_buffer.get_normals();
        normal_offset_ += add_buffer_data(normals, normal_vbo, GL_ARRAY_BUFFER, normal_offset_);

        auto& colors = draw_buffer.get_colours();
        color_offset_ += add_buffer_data(colors, color_vbo, GL_ARRAY_BUFFER, color_offset_);

        auto& indices = draw_buffer.get_indices();
        index_offset_ += add_buffer_data(indices, index_vbo, GL_ELEMENT_ARRAY_BUFFER, index_offset_);
        points_to_draw_ += indices.size();
    }
}

void ConcreteRenderer::_set_gpu_vertex_attributes() {

    VertexAttribPointerConfig constexpr buffer_config{
        .offset = 0,
        .size = 3,
        .stride = 0,
        .data_type = GL_FLOAT,
        .buffer_type = GL_ARRAY_BUFFER,
        .normalized = false,
    };
    
    auto const vertex_vbo = vbos_[BufferType::Vertex];
    configure_vertex_attrib(shader_program_->address, vertex_vbo, "aPos", buffer_config);
}

void ConcreteRenderer::_set_gpu_normal_attributes() {

    VertexAttribPointerConfig constexpr buffer_config{
        .offset = 0,
        .size = 3,
        .stride = 0,
        .data_type = GL_FLOAT,
        .buffer_type = GL_ARRAY_BUFFER,
        .normalized = false,
    };

    auto const normal_vbo = vbos_[BufferType::Normal];
    configure_vertex_attrib(shader_program_->address, normal_vbo, "aNor", buffer_config);
}

void ConcreteRenderer::_set_gpu_colour_attributes() {

    VertexAttribPointerConfig constexpr buffer_config{
        .offset = 0,
        .size = 4,
        .stride = 0,
        .data_type = GL_FLOAT,
        .buffer_type = GL_ARRAY_BUFFER,
        .normalized = true,
    };
    auto const color_vbo = vbos_[BufferType::Color];
    configure_vertex_attrib(shader_program_->address, color_vbo, "aCol", buffer_config);
}

// void ConcreteRenderer::_set_gpu_index_attributes() {

//     VertexAttribPointerConfig constexpr buffer_config{
//         .offset = 0,
//         .size = 4,
//         .stride = 0,
//         .data_type = GL_FLOAT,
//         .buffer_type = GL_ELEMENT_ARRAY_BUFFER,
//         .normalized = true,
//     };
//     auto const index_vbo = vbos_[BufferType::Index];
//     configure_vertex_attrib(shader_program_->address, index_vbo, "aCol", buffer_config);
// }

// TODO : Ideally, we want to pass a list of uniforms to be set here (variant)
void ConcreteRenderer::render(const glm::mat4& proj, const glm::mat4& view, glm::mat4 const& rot, std::span<float const> heights, std::span<glm::vec3 const> offsets) {
    auto errors = getError();

    shader_program_->use();

    // float green       = std::abs(0.6 + sin(glfwGetTime() * 2) / 2.0);
    // glm::mat4 rotate1 = glm::rotate((float) sin(glfwGetTime() * 2) * 3.14159f, glm::vec3(1, 0, 0));
    // glm::mat4 rotate2 = glm::rotate((float) cos(glfwGetTime() * 2) * 3.14159f, glm::vec3(0, 1, 0));

    shader_program_->set_uniform("proj", proj);
    shader_program_->set_uniform("view", view);
    shader_program_->set_uniform("rotate", rot);

    shader_program_->set_uniform("heights", {heights});
    shader_program_->set_uniform("offsets", {offsets});


    // Enabling some features
    errors = getError();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    errors = getError();

    _enable_vertex_attributes();
    errors = getError();

    // Enable vertices and normals for drawing
    glDrawElementsInstanced(GL_TRIANGLES, points_to_draw_, GL_UNSIGNED_INT, 0, 100);
    errors = getError();
    // glDrawArrays(GL_TRIANGLES, 0, points_to_draw_);


    _disable_vertex_attributes();
    errors = getError();

    shader_program_->unuse();
    errors = getError();
    errors = getError();
}

void ConcreteRenderer::_enable_vertex_attributes() {
    glBindVertexArray(vao_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos_[BufferType::Index]);

    // Get the layout locations
    GLuint vertexPtr = glGetAttribLocation(shader_program_->address, "aPos");
    glEnableVertexAttribArray(vertexPtr);

    GLuint normalPtr = glGetAttribLocation(shader_program_->address, "aNor");
    glEnableVertexAttribArray(normalPtr);

    GLuint colourPtr = glGetAttribLocation(shader_program_->address, "aCol");
    glEnableVertexAttribArray(colourPtr);
}

void ConcreteRenderer::_disable_vertex_attributes() {
    // Get the layout locations
    GLuint vertexPtr = glGetAttribLocation(shader_program_->address, "aPos");
    glDisableVertexAttribArray(vertexPtr);

    GLuint normalPtr = glGetAttribLocation(shader_program_->address, "aNor");
    glDisableVertexAttribArray(normalPtr);

    GLuint colourPtr = glGetAttribLocation(shader_program_->address, "aCol");
    glDisableVertexAttribArray(colourPtr);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void ConcreteRenderer::queue_data(DrawBuffer&& buffer) {
    // TODO : Before, this was actually tying the data to
    // TODO : An entity, so we may want to tie the data to
    // TODO : a mesh component in the future.

    // TODO : Is this the right call here for efficiency?
    pending_buffers_.emplace(std::move(buffer));
}
