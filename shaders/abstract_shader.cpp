// Includes from this project
#include <shaders/abstract_shader.hpp>
#include <shaders/shader.h>

// Inclues from third party
#include <GL/glew.h>

AbstractShader::AbstractShader(ShaderData const &data)
    : data_{data}, address_{0} {}

bool AbstractShader::compile() {

  // Check if this shader has already been
  // allocated in the GPU, if so, delete it
  if (address_ != 0) {
    glDeleteShader(address_);
  }

  // TODO : We have to get the virtual type of the shader
  // TODO : in a GLEnum format
  address_ = glCreateShader(get_shader_type());
  if (address_ == 0) {
    return false;
  }

  auto const data_size = static_cast<int>(data_.glsl.size());
  auto *data_ptr = data_.glsl.data();
  glShaderSource(address_, 1, &data_ptr, &data_size);
  glCompileShader(address_);

  return true;
}

unsigned int AbstractShader::get_address() const { return address_; }

std::string AbstractShader::get_compilation_message() const {
  if (address_ != 0) {
    constexpr unsigned int buffer_size = 512;
    char message_buffer[buffer_size];

    int retrieve_status;
    glGetShaderiv(address_, GL_COMPILE_STATUS, &retrieve_status);
    if (!retrieve_status) {
      glGetShaderInfoLog(address_, buffer_size, NULL, message_buffer);
      return message_buffer; // Should be converted to str
    } else {
      return "compilation successful";
    }
  } else {
    return "shader has not been compiled yet";
  }
}