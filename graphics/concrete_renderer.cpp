#include <alloca.h>
#include <shaders/shader_program.h>

#include <glm/fwd.hpp>
#include <graphics/concrete_renderer.hpp>
#include <graphics/draw_buffer.hpp>

// #include "GLAbstractions/vao.h"
// #include "GLAbstractions/vbo.h"
// #include "GLAbstractions/vertex_attribute.h"

// Includes from third party
#include <GL/gl.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

// Includes from the STD
#include <cmath>
#include <exception>
#include <optional>
#include <span>


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

    std::optional<std::uint32_t> create_vao() {
        std::uint32_t array_id = 0;
        glGenVertexArrays(1, &array_id);

        if (array_id == 0) {
            return std::nullopt;
        }

        return array_id;
    }

    void delete_vao(std::uint32_t array_id) {
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
        glDeleteBuffers(1, &buffer_id);
    }

    void allocate_buffer_memory(std::uint32_t vbo, std::uint32_t size) {

        if (vbo == 0) {
            // TODO : Probably want to log something here
            return;
        }

        // TODO : Do we want to just provide a function that will copy
        // TODO : the data straight away? We're running an allocation THEN
        // TODO : a copy.
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    // TODO : This void* will be complained at, maybe we use a span???
	// TODO : This should probably return an error in case data is larger than buffer
    void add_buffer_data(std::span<std::uint8_t> data, std::uint32_t vbo, const std::uint32_t offset) {

        if (vbo == 0) {
            // TODO : Probably want to do some loggind here
            return;
        }

        // TODO : These can throw a lot of errors, make sure we catch them
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, offset, data.size(), reinterpret_cast<void*>(data.data()));
    }

	template <typename T>
	std::uint32_t add_vector_data(std::vector<T>& vertices, std::uint32_t vbo, std::uint32_t offset) {

		std::uint32_t const vertices_byte_size = vertices.size() * sizeof(T);
		std::span<std::uint8_t> const vertices_span{reinterpret_cast<std::uint8_t*>(&vertices[0]), vertices_byte_size};
		add_buffer_data(vertices_span, vbo, offset);
		return vertices_byte_size;
	}
} // namespace


ConcreteRenderer::ConcreteRenderer() : vao_(0), shader_address_{0}, points_to_draw_{0}, height_{1}, offset_{0} {

    auto const vao_address = create_vao();

    if (!vao_address) {
        throw VaoCreationException();
    }

    vao_ = *vao_address;

    // insert the stuff in vbos
    auto vertex_vbo = std::make_unique<VBO>();
    vbos_.insert({BufferType::Vertex, std::move(vertex_vbo)});
    auto normal_vbo = std::make_unique<VBO>();
    vbos_.insert({BufferType::Normal, std::move(normal_vbo)});
    auto colour_vbo = std::make_unique<VBO>();
    vbos_.insert({BufferType::Color, std::move(colour_vbo)});
}

ConcreteRenderer::~ConcreteRenderer() {
    delete_vao(vao_);

    for (auto const& [buffer_type, vbo] : vbos_) {
        delete_vbo(vbo);
    }
}

void ConcreteRenderer::set_shader(unsigned int address) {
    shader_address_ = address;
}

void ConcreteRenderer::send_gpu_data() {
    points_to_draw_ = 0;

    if (shader_address_ != 0) {
        glUseProgram(shader_address_);
    }

    // glBindVertexArray(vao_->getId());

    std::vector<std::string> errors;

    // Allocate enough memory at the buffers
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

    get_shader()->unuse();
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
    constexpr std::uint32_t DEFAULT_BUFFER_SIZE = sizeof(float) * 2'000'000;

    auto const vertex_address = vbos_[BufferType::Vertex];
    auto const normal_address = vbos_[BufferType::Normal];
    auto const color_vbo      = vbos_[BufferType::Color];

    allocate_buffer_memory(vertex_address, DEFAULT_BUFFER_SIZE);
    allocate_buffer_memory(normal_address, DEFAULT_BUFFER_SIZE);
    allocate_buffer_memory(color_vbo, DEFAULT_BUFFER_SIZE);
}

void ConcreteRenderer::_populate_gpu_buffers() {
    auto const vertex_vbo = vbos_[BufferType::Vertex];
    auto const normal_vbo = vbos_[BufferType::Normal];
    auto const color_vbo  = vbos_[BufferType::Color];

	while (!pending_buffers_.empty()) {
		auto draw_buffer = std::move(pending_buffers_.front());
		pending_buffers_.pop();

		// vertex data
		auto& vertices = draw_buffer.get_vertices();
		vertex_offset_ += add_vector_data(vertices, vertex_vbo, vertex_offset_);

		auto& normals = draw_buffer.get_normals();
		normal_offset_ += add_vector_data(normals, normal_vbo, normal_offset_);

		auto& colors = draw_buffer.get_colours();
		color_offset_ += add_vector_data(colors, color_vbo, color_offset_);
	}
}

void ConcreteRenderer::_set_gpu_vertex_attributes() {
    VBO* vertex_vbo = vbos_[BufferType::VERTEX].get();
    glBindBuffer(GL_ARRAY_BUFFER, vertex_vbo->getId());

    auto vertex_attrib = std::make_unique<VertexAttribute>();
    vertex_attrib->setOffset(0);
    vertex_attrib->setSize(3); // x,y,y
    vertex_attrib->setNormalised(false);
    vertex_attrib->setStride(0);
    vertex_attrib->setType(VERTEX_TYPE::FLOAT);

    // Get the layout location
    GLint posPtr = glGetAttribLocation(get_shader()->get_address(), "aPos");

    glVertexAttribPointer(posPtr, vertex_attrib->getSize(), (int) vertex_attrib->getType(),
        vertex_attrib->getNormalised(), vertex_attrib->getStride(), (GLvoid*) vertex_attrib->getOffset());

    vao_->addBufferConfigs(vertex_vbo, std::move(vertex_attrib));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ConcreteRenderer::_set_gpu_normal_attributes() {
    VBO* normal_vbo = vbos_[BufferType::NORMAL].get();
    glBindBuffer(GL_ARRAY_BUFFER, normal_vbo->getId());

    auto normal_attrib = std::make_unique<VertexAttribute>();
    normal_attrib->setOffset(0);
    normal_attrib->setSize(3); // xn, yn, zn
    normal_attrib->setNormalised(false);
    normal_attrib->setStride(0);
    normal_attrib->setType(VERTEX_TYPE::FLOAT);

    // Get the layout location for normals
    GLint norPtr = glGetAttribLocation(get_shader()->get_address(), "aNor");

    glVertexAttribPointer(norPtr, normal_attrib->getSize(), (int) normal_attrib->getType(),
        normal_attrib->getNormalised(), normal_attrib->getStride(), (GLvoid*) normal_attrib->getOffset());

    vao_->addBufferConfigs(normal_vbo, std::move(normal_attrib));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ConcreteRenderer::_set_gpu_colour_attributes() {
    auto error      = getError();
    VBO* colour_vbo = vbos_[BufferType::COLOUR].get();
    glBindBuffer(GL_ARRAY_BUFFER, colour_vbo->getId());
    error = getError();

    auto colour_attribute = std::make_unique<VertexAttribute>();
    colour_attribute->setOffset(0);
    colour_attribute->setSize(4); // r, g, b, a
    colour_attribute->setNormalised(true); // Just in case its outside the range
    colour_attribute->setStride(0);
    colour_attribute->setType(VERTEX_TYPE::FLOAT);

    // Get the layout location
    GLint colPtr = glGetAttribLocation(get_shader()->get_address(), "aCol");
    error        = getError();

    glVertexAttribPointer(colPtr, colour_attribute->getSize(), (int) colour_attribute->getType(),
        colour_attribute->getNormalised(), colour_attribute->getStride(), (GLvoid*) colour_attribute->getOffset());
    error = getError();

    vao_->addBufferConfigs(colour_vbo, std::move(colour_attribute));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    error = getError();
}

void ConcreteRenderer::render(const glm::mat4& proj, const glm::mat4& view) {
    auto errors = getError();

    //! The last piece of the puzzle. Problem is to
    //! think about how to link the attributes and
    //! buffers in the VAO. Specifically, where is the
    //! best place to send the information to the GPU?
    //! Would it be here or in the VAO object itself?
    get_shader()->use();

    float green       = abs(0.6 + sin(glfwGetTime() * 2) / 2.0);
    glm::mat4 rotate1 = glm::rotate((float) sin(glfwGetTime() * 2) * 3.14159f, glm::vec3(1, 0, 0));
    glm::mat4 rotate2 = glm::rotate((float) cos(glfwGetTime() * 2) * 3.14159f, glm::vec3(0, 1, 0));

    get_shader()->set_uniform("proj", proj);
    get_shader()->set_uniform("view", view);
    get_shader()->set_uniform("rotate", glm::mat4(1));
    get_shader()->set_uniform("height", height_);
    get_shader()->set_uniform("offset", offset_);

    // Enabling some features
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glBindVertexArray(vao_->getId());
    _enable_gpu_buffers();

    // Enable vertices and normals for drawing
    glDrawArrays(GL_TRIANGLES, 0, points_to_draw_);
    glBindVertexArray(0);


    _disable_gpu_buffers();
    get_shader()->unuse();
    errors = getError();
}

void ConcreteRenderer::_enable_gpu_buffers() {
    // Get the layout locations
    GLuint vertexPtr = glGetAttribLocation(get_shader()->get_address(), "aPos");
    glEnableVertexAttribArray(vertexPtr);

    GLuint normalPtr = glGetAttribLocation(get_shader()->get_address(), "aNor");
    glEnableVertexAttribArray(normalPtr);

    GLuint colourPtr = glGetAttribLocation(get_shader()->get_address(), "aCol");
    glEnableVertexAttribArray(colourPtr);
}

void ConcreteRenderer::_disable_gpu_buffers() {
    // Get the layout locations
    GLuint vertexPtr = glGetAttribLocation(get_shader()->get_address(), "aPos");
    glDisableVertexAttribArray(vertexPtr);

    GLuint normalPtr = glGetAttribLocation(get_shader()->get_address(), "aNor");
    glDisableVertexAttribArray(normalPtr);

    GLuint colourPtr = glGetAttribLocation(get_shader()->get_address(), "aCol");
    glDisableVertexAttribArray(colourPtr);
}


void ConcreteRenderer::queue_data(DrawBufferPtr&& buffer) {
    // TODO : Before, this was actually tying the data to
    // TODO : An entity, so we may want to tie the data to
    // TODO : a mesh component in the future.

    // TODO : Is this the right call here for efficiency?
    pending_buffers_.emplace(std::move(buffer));
}


// TODO : Need to resize these so we get this information from
// TODO : the queue
unsigned ConcreteRenderer::_get_vertex_size() const {
    unsigned total = 0;
    for (auto& kvPair : entity_data_) {
        for (auto& dataPtr : kvPair.second) {
            total += dataPtr->get_vertices().getGPUSize();
        }
    }
    return total;
}

unsigned ConcreteRenderer::_get_normal_size() const {
    unsigned total = 0;
    for (auto& kvPair : entity_data_) {
        for (auto& dataPtr : kvPair.second) {
            total += dataPtr->get_normals().getGPUSize();
        }
    }
    return total;
}

unsigned ConcreteRenderer::_get_colour_size() const {
    unsigned total = 0;
    for (auto& kvPair : entity_data_) {
        for (auto& dataPtr : kvPair.second) {
            total += dataPtr->get_colours().getGPUSize();
        }
    }
    return total;
}

void ConcreteRenderer::set_height(float h) {
    height_ = h;
}

void ConcreteRenderer::set_offset(float h) {
    offset_ = h;
}
