#include <graphics/concrete_renderer.hpp>

// #include "GLAbstractions/vao.h"
// #include "GLAbstractions/vbo.h"
// #include "GLAbstractions/vertex_attribute.h"

#include "Graphics/draw_buffer.h"
#include "Shaders/shader_program.h"

// Includes from third party
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

// Includes from the STD
#include <cmath>


std::vector<std::string> getError()
{
	std::vector<std::string> retVal;
	GLenum error;
	while ((error = glGetError()) != GL_NO_ERROR) {

		switch (error)
		{
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

ConcreteRenderer::ConcreteRenderer()
	: 
	vao_(std::make_unique<VAO>()),
    shader_address_{0},
	points_to_draw_{ 0 },
	height_ { 1 },
	offset_ { 0 }
{
	// insert the stuff in vbos
	auto vertex_vbo = std::make_unique<VBO>();
	vbos_.insert({BUFFER_TYPE::VERTEX, std::move(vertex_vbo)});
	auto normal_vbo = std::make_unique<VBO>();
	vbos_.insert({BUFFER_TYPE::NORMAL, std::move(normal_vbo)});
	auto colour_vbo = std::make_unique<VBO>();
	vbos_.insert({BUFFER_TYPE::COLOUR,std::move(colour_vbo)});
}

void ConcreteRenderer::set_shader(unsigned int address)
{
	shader_address_ = address;
}

void ConcreteRenderer::send_gpu_data()
{
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

void ConcreteRenderer::_allocate_gpu_memory()
{
	//! Assumes we have already bound the 
	//! vertex array object
	VBO* vertexVBO = vbos_[BUFFER_TYPE::VERTEX].get();
	vertexVBO->allocateMemory(_get_vertex_size());
	VBO* normal_vbo = vbos_[BUFFER_TYPE::NORMAL].get();
	normal_vbo->allocateMemory(_get_normal_size());
	VBO* colour_vbo = vbos_[BUFFER_TYPE::COLOUR].get();
	colour_vbo->allocateMemory(_get_colour_size());
}

void ConcreteRenderer::_populate_gpu_buffers()
{
	VBO* vertexVBO = vbos_[BUFFER_TYPE::VERTEX].get();
	VBO* normal_vbo = vbos_[BUFFER_TYPE::NORMAL].get();
	VBO* colour_vbo = vbos_[BUFFER_TYPE::COLOUR].get();

	points_to_draw_= 0;
	unsigned vertexOffset = 0;
	unsigned normalOffset = 0;
	unsigned colourOffset = 0;
	for (auto& kvPair : entity_data_) {
		for (auto& buffer : kvPair.second) {
			vertexVBO->addData(buffer->get_vertices().getData(), vertexOffset);
			vertexOffset += buffer->get_vertices().getGPUSize();
			normal_vbo->addData(buffer->get_normals().getData(), normalOffset);
			normalOffset += buffer->get_normals().getGPUSize();
			colour_vbo->addData(buffer->get_colours().getData(), colourOffset);
			colourOffset += buffer->get_colours().getGPUSize();

			points_to_draw_ += buffer->get_vertices().getData().size();
		}
	}
	// TODO : figure out how the ssbos will be sent
	// for the entity transformations
}

void ConcreteRenderer::_set_gpu_vertex_attributes()
{
	VBO* vertex_vbo = vbos_[BUFFER_TYPE::VERTEX].get();
	glBindBuffer(GL_ARRAY_BUFFER, vertex_vbo->getId());

	auto vertex_attrib = std::make_unique<VertexAttribute>();
	vertex_attrib->setOffset(0);
	vertex_attrib->setSize(3); // x,y,y
	vertex_attrib->setNormalised(false);
	vertex_attrib->setStride(0);
	vertex_attrib->setType(VERTEX_TYPE::FLOAT);

	// Get the layout location
	GLint posPtr = glGetAttribLocation(get_shader()->get_address(), "aPos");

	glVertexAttribPointer(
		posPtr,
		vertex_attrib->getSize(),
		(int) vertex_attrib->getType(),
		vertex_attrib->getNormalised(),
		vertex_attrib->getStride(),
		(GLvoid*) vertex_attrib->getOffset()
		);

	vao_->addBufferConfigs(vertex_vbo, std::move(vertex_attrib));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ConcreteRenderer::_set_gpu_normal_attributes()
{
	VBO* normal_vbo = vbos_[BUFFER_TYPE::NORMAL].get();
	glBindBuffer(GL_ARRAY_BUFFER, normal_vbo->getId());

	auto normal_attrib = std::make_unique<VertexAttribute>();
	normal_attrib->setOffset(0);
	normal_attrib->setSize(3); // xn, yn, zn
	normal_attrib->setNormalised(false);
	normal_attrib->setStride(0);
	normal_attrib->setType(VERTEX_TYPE::FLOAT);

	// Get the layout location for normals
	GLint norPtr = glGetAttribLocation(get_shader()->get_address(), "aNor");

	glVertexAttribPointer(
		norPtr,
		normal_attrib->getSize(),
		(int)normal_attrib->getType(),
		normal_attrib->getNormalised(),
		normal_attrib->getStride(),
		(GLvoid*)normal_attrib->getOffset()
		);

	vao_->addBufferConfigs(normal_vbo, std::move(normal_attrib));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ConcreteRenderer::_set_gpu_colour_attributes()
{
	auto error = getError();
	VBO* colour_vbo = vbos_[BUFFER_TYPE::COLOUR].get();
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
	error = getError();

	glVertexAttribPointer(
		colPtr,
		colour_attribute->getSize(),
		(int)colour_attribute->getType(),
		colour_attribute->getNormalised(),
		colour_attribute->getStride(),
		(GLvoid*)colour_attribute->getOffset()
		);
	error = getError();

	vao_->addBufferConfigs(colour_vbo, std::move(colour_attribute));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	error = getError();
}

void ConcreteRenderer::render(const glm::mat4& proj, const glm::mat4& view)
{
	auto errors = getError();

	//! The last piece of the puzzle. Problem is to
	//! think about how to link the attributes and
	//! buffers in the VAO. Specifically, where is the
	//! best place to send the information to the GPU?
	//! Would it be here or in the VAO object itself?
	get_shader()->use();

	float green = abs(0.6 + sin(glfwGetTime() * 2) / 2.0);
	glm::mat4 rotate1 = glm::rotate((float)sin(glfwGetTime() * 2) * 3.14159f, glm::vec3(1, 0, 0));
	glm::mat4 rotate2 = glm::rotate((float)cos(glfwGetTime() * 2) * 3.14159f, glm::vec3(0, 1, 0));

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

void ConcreteRenderer::_enable_gpu_buffers()
{
	// Get the layout locations
	GLuint vertexPtr = glGetAttribLocation(get_shader()->get_address(), "aPos");
	glEnableVertexAttribArray(vertexPtr);

	GLuint normalPtr = glGetAttribLocation(get_shader()->get_address(), "aNor");
	glEnableVertexAttribArray(normalPtr);

	GLuint colourPtr = glGetAttribLocation(get_shader()->get_address(), "aCol");
	glEnableVertexAttribArray(colourPtr);
}

void ConcreteRenderer::_disable_gpu_buffers()
{
	// Get the layout locations
	GLuint vertexPtr = glGetAttribLocation(get_shader()->get_address(), "aPos");
	glDisableVertexAttribArray(vertexPtr);

	GLuint normalPtr = glGetAttribLocation(get_shader()->get_address(), "aNor");
	glDisableVertexAttribArray(normalPtr);

	GLuint colourPtr = glGetAttribLocation(get_shader()->get_address(), "aCol");
	glDisableVertexAttribArray(colourPtr);
}


void ConcreteRenderer::add_entity_data(EntityPtr entPtr, DrawBufferPtr buffer)
{
	auto& found = entity_data_.find(entPtr);
	if (found != end(entity_data_)) {
		found->second.push_back(std::move(buffer));
	}
	else {
		std::vector<DrawBufferPtr> val;
		val.push_back(std::move(buffer));
		entity_data_.insert(std::make_pair(std::move(entPtr), std::move(val)));
	}
}


unsigned ConcreteRenderer::_get_vertex_size() const
{
	unsigned total = 0;
	for (auto& kvPair : entity_data_) {
		for (auto& dataPtr : kvPair.second) {
			total += dataPtr->get_vertices().getGPUSize();
		}
	}
	return total;
}

unsigned ConcreteRenderer::_get_normal_size() const
{
	unsigned total = 0;
	for (auto& kvPair : entity_data_) {
		for (auto& dataPtr : kvPair.second) {
			total += dataPtr->get_normals().getGPUSize();
		}
	}
	return total;
}

unsigned ConcreteRenderer::_get_colour_size() const
{
	unsigned total = 0;
	for (auto& kvPair : entity_data_) {
		for (auto& dataPtr : kvPair.second) {
			total += dataPtr->get_colours().getGPUSize();
		}
	}
	return total;
}

void ConcreteRenderer::set_height(float h)
{
	height_ = h;
}

void ConcreteRenderer::set_offset(float h)
{
	offset_ = h;
}
