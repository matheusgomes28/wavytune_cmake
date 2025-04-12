#ifndef RENDERER_CONCRETE_RENDERER_H
#define RENDERER_CONCRETE_RENDERER_H

// Includes from OpenGL
#include <graphics/abstract_renderer.hpp>

// Includes from the std
#include <cstdint>
#include <exception>
#include <map>
#include <queue>
#include <vector>

class DrawBuffer;
class ShaderProgram;

/**
 * @brief Represents type of draw data supported
 * by the renderer:
 * 
 *  + Vertex: x,y,z packed vector or
 *
 * floats.
 *  + Normal: x,y,z packed vector or floats.
 *  + Texture: u,v packed vector or floats.
 *  + Color:
 * r,g,b,a
 * packed vector or floats.
 */
enum class BufferType { Vertex, Normal, Texture, Color };


/**
 * @brief esception thrown when the renderer
 * cannot create a VAO object
 */
class VaoCreationException : public std::exception {
    virtual const char* what() const throw() {
        return "Could not creat Vertex Array Object";
    }
};


class ConcreteRenderer : public AbstractRenderer {

public:
    ConcreteRenderer();
    ~ConcreteRenderer();


    //! overrides
    void render(const glm::mat4& proj, const glm::mat4& view) override;
    void send_gpu_data() override;
    void queue_data(DrawBufferPtr&& dataPtr) override;
    void set_shader(unsigned int address) override;

    // TODO : Wtf are these for?
    void set_offset(float offset);
    void set_height(float height);


private:
    std::vector<DrawBufferPtr> draw_buffers_;

    /**
     * @brief Map of buffer type to VBO data to be drawn on
     * the screen.
     */
    std::map<BufferType, std::uint32_t> vbos_;

    /**
     * @brief GL id for the underlying VAO for this renderer
     * @note each render only supports one VAO,
     * which has many
     * VBOs.
     */
    std::uint32_t vao_;

    /**
     * @brief Queue of draw data to be sent to the VBO, which
     * will happen everytime `_populate_gpu_buffers` is called.
     */
    std::queue<DrawBuffer> pending_buffers_;

    unsigned int shader_address_;
    unsigned points_to_draw_;

    void _allocate_gpu_memory();
    void _populate_gpu_buffers();
    void _set_gpu_vertex_attributes();
    void _set_gpu_normal_attributes();
    void _set_gpu_colour_attributes();
    void _enable_gpu_buffers();
    void _disable_gpu_buffers();

    /**
     * @brief number of currently loaded vertex 
     */
    std::uint32_t vertex_offset_;
    std::uint32_t normal_offset_;
    std::uint32_t color_offset_;
    // std::uint32_t texel_offset_;


    // TODO : These are here for testing, remove them in prod
    float height_;
    float offset_;
};

#endif // RENDERER_CONCRETE_RENDERER_H
