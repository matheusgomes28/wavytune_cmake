#include <graphics/Shader.h>

#include <GL/glew.h>
#include <gsl/gsl>

#include <array>
#include <string>

using namespace wt;

ShaderException::ShaderException(std::string message)
    : std::runtime_error(message), _message(std::move(message))
{
}

const char *ShaderException::what() const noexcept { return _message.c_str(); }

Shader::Shader(ByteVector const &from, ShaderType const &type)
    : _address(0), _source_code(from), _type(type)
{
}

Shader::Shader(ByteVector &&from, ShaderType const &type)
    : _address(0), _source_code(std::move(from)), _type(type)
{
}

bool Shader::compile()
{
  // If already compiled, delete from GPU
  // first to recompile
  if (_address)
  {
    glDeleteShader(_address);
  }

  _address = glCreateShader(static_cast<GLenum>(_type));

  if (_address)
  {
    auto str_data = reinterpret_cast<const char *>(_source_code.data());
    int data_size = gsl::narrow_cast<int>(_source_code.size());
    glShaderSource(_address, 1, &str_data, &data_size);
    glCompileShader(_address);
  }
  else
  {
    throw ShaderException("could not allocate a shader");
  }
}

void Shader::flag_delete()
{
  if (_address)
  {
    glDeleteShader(_address);
    _address = 0;
  }
}

unsigned int Shader::get_address() const { return _address; }

std::string Shader::get_compilation_message() const
{
  if (_address)
  {
    const unsigned int buffer_size = 512;
    std::array<char, buffer_size> message_buffer;

    int retrieve_status;
    glGetShaderiv(_address, GL_COMPILE_STATUS, &retrieve_status);

    if (!retrieve_status)
    {
      glGetShaderInfoLog(_address, buffer_size, NULL, message_buffer.data());
      return message_buffer.data(); // Should be converted to str
    }

    return "compilation successful";
  }

  return "shader has not been compiled yet";
}