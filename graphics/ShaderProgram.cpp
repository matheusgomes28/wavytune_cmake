#include <graphics/Shader.h>
#include <graphics/ShaderProgram.h>

#include <glad/glad.h>
#include <gsl/gsl_assert>

#include <iostream>
#include <memory>
#include <string>

namespace
{
  void compile_shader(wt::Shader &shader) { shader.compile(); }

  void compile_shaders_if_necessary(wt::ShaderPointer &fs,
                                    wt::ShaderPointer &gs,
                                    wt::ShaderPointer &vs)
  {
    // Need vertex and fragment shaders but
    // geometry is optional
    Expects(fs);
    compile_shader(*fs);
    Expects(vs);
    compile_shader(*vs);
    if (gs)
    {
      compile_shader(*gs);
    }
  }

  void log_shader_compilation_message(wt::ShaderPointer const &fs,
                                      wt::ShaderPointer const &gs,
                                      wt::ShaderPointer const &vs)
  {
    std::cout << "Fragment Shader: " << fs->get_compilation_message()
              << std::endl;
    if (gs)
    {
      std::cout << "Geometry Shader: " << gs->get_compilation_message()
                << std::endl;
    }
    std::cout << "Vertex Shader: " << vs->get_compilation_message()
              << std::endl;
  }

  void attach_shaders(unsigned int program_id, unsigned int fs_id,
                      unsigned int vs_id)
  {
    Expects(program_id != 0);
    Expects(fs_id != 0);
    Expects(vs_id != 0);
    glAttachShader(program_id, fs_id);
    glAttachShader(program_id, vs_id);
  }

  void attach_shaders(unsigned int program_id, unsigned int fs_id,
                      unsigned int gs_id, unsigned int vs_id)
  {
    attach_shaders(program_id, fs_id, vs_id);
    Expects(gs_id != 0);
    glAttachShader(program_id, gs_id);
  }

  void link_program(unsigned int program_id)
  {
    Expects(program_id);
    glLinkProgram(program_id);
  }

  void detach_shaders(unsigned int program_id, unsigned int fs_id,
                      unsigned int vs_id)
  {
    Expects(program_id != 0);
    Expects(fs_id != 0);
    Expects(vs_id != 0);
    glDetachShader(program_id, fs_id);
    glDetachShader(program_id, vs_id);
  }

  void detach_shaders(unsigned int program_id, unsigned int fs_id,
                      unsigned int gs_id, unsigned int vs_id)
  {
    detach_shaders(program_id, fs_id, vs_id);
    Expects(gs_id != 0);
    glDetachShader(program_id, gs_id);
  }

  unsigned int generate_program_address() { return glCreateProgram(); }

  unsigned int get_current_program_address()
  {
    int ret;
    glGetIntegerv(GL_CURRENT_PROGRAM, &ret);
    return ret;
  }

} // namespace

using namespace wt;

ShaderProgramException::ShaderProgramException(std::string error_message)
    : std::runtime_error{error_message}, _message{error_message}
{
}

ShaderProgram::ShaderProgram(ShaderPointer fragment_shader,
                             ShaderPointer vertex_shader)
    : _address(0), _fs{std::move(fragment_shader)}, _vs{std::move(
                                                        vertex_shader)}
{
}

ShaderProgram::ShaderProgram(ShaderPointer fragment_shader,
                             ShaderPointer geometry_shader,
                             ShaderPointer vertex_shader)
    : _address(0), _fs{std::move(fragment_shader)},
      _gs{std::move(geometry_shader)}, _vs{std::move(vertex_shader)}
{
}

ShaderProgram::~ShaderProgram()
{
  // Delete if the address exists
  if (_address)
  {
    glDeleteProgram(_address);
  }
}

void ShaderProgram::set_fragment_shader(std::unique_ptr<Shader> fs)
{
  _fs = std::move(fs);
}

void ShaderProgram::set_geometry_shader(std::unique_ptr<Shader> gs)
{
  _gs = std::move(gs);
}

void ShaderProgram::set_vertex_shader(std::unique_ptr<Shader> vs)
{
  _vs = std::move(vs);
}

void ShaderProgram::compile_and_link()
{
  _address = generate_program_address();
  if (_address == 0)
  {
    throw ShaderProgramException("could not create the shader program");
  }

  compile_shaders_if_necessary(_fs, _gs, _vs);
  log_shader_compilation_message(_fs, _gs, _vs);

  // Link all the shaders then detach for memory deletion
  if (_gs)
  {
    attach_shaders(_address, _fs->get_address(), _gs->get_address(),
                   _vs->get_address());
    link_program(_address);
    detach_shaders(_address, _fs->get_address(), _gs->get_address(),
                   _vs->get_address());
  }
  else
  {
    attach_shaders(_address, _fs->get_address(), _vs->get_address());
    link_program(_address);
    detach_shaders(_address, _fs->get_address(), _vs->get_address());
  }
}

void ShaderProgram::use() const
{
  if (_address)
  {
    glUseProgram(_address);
  }
  else
  {
    throw std::logic_error("Cannot use uncompiled shader program.");
  }
}

void ShaderProgram::unuse() const
{
  if (get_current_program_address() == _address)
  {
    glUseProgram(0);
  }
}
