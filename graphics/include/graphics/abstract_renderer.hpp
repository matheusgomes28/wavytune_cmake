#ifndef RENDERERS_ABSTRACT_RENDERER_H
#define RENDERERS_ABSTRACT_RENDERER_H

// Includes from the STD
#include <memory>

// Includes from third party libs
#include <glm/glm.hpp>

class ShaderProgram;
class DrawBuffer;

//! Class that defines the interface of a render.
//! Note that all the renderes in this program 
//! will inherit from this class.
class AbstractRenderer
{
public:
	using DrawBufferPtr = std::unique_ptr<DrawBuffer>;

	virtual void render(const glm::mat4& proj, const glm::mat4& view) = 0;
	virtual void send_gpu_data() = 0;
	virtual void queue_data(DrawBufferPtr&& data) = 0;
	virtual void set_shader(unsigned int shader_address) = 0;

};
#endif