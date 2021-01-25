#ifndef GRAPHICS_SHADER_H
#define GRAPHICS_SHADER_H

#include <data_structures/bytes.h>

#include <GL/glew.h>

#include <stdexcept>
#include <string>

namespace wt
{
  enum class ShaderType : unsigned int
  {
    FRAGMENT = GL_FRAGMENT_SHADER,
    GEOMETRY = GL_GEOMETRY_SHADER,
    VERTEX = GL_VERTEX_SHADER
  };

  class Shader
  {
  public:
    Shader(wt::ByteVector const &from, ShaderType const &type);
    Shader(wt::ByteVector &&from, ShaderType const &type);

    bool compile();
    void flag_delete();
    unsigned int get_address() const;
    std::string get_compilation_message() const;

  private:
    unsigned int _address;
    ByteVector _source_code;
    ShaderType _type;
  };

  class ShaderException : std::runtime_error
  {
  public:
    ShaderException(std::string err_message);
    const char *what() const noexcept;

  private:
    std::string _message;
  };
} // namespace wt

#endif // GRAPHICS_SHADER_H