#ifndef GRAPHICS_SHADER_PROGRAM_H
#define GRAPHICS_SHADER_PROGRAM_H

#include <memory>
#include <stdexcept>
#include <string>

#include <glm/glm.hpp>

namespace wt
{
	class Shader;
	using ShaderPointer = std::unique_ptr<Shader>;

	class ShaderProgram
	{
	public:

		ShaderProgram(ShaderPointer fragment_shader, ShaderPointer vertex_shader);
		ShaderProgram(ShaderPointer fragment_shader, ShaderPointer geometry_shader, ShaderPointer vertex_shader);
		~ShaderProgram();

		void set_fragment_shader(std::unique_ptr<Shader> fs);
		void set_geometry_shader(std::unique_ptr<Shader> gs);
		void set_vertex_shader(std::unique_ptr<Shader> vs);

		void set_uniform(std::string const& name, int const& value) const;
		void set_uniform(std::string const& name, unsigned int const& value) const;
		void set_uniform(std::string const& name, float const& value) const;
		void set_uniform(std::string const& name, double const& value) const;
		void set_uniform(std::string const& name, glm::vec2 const& value) const;
		void set_uniform(std::string const& name, glm::vec3 const& value) const;
		void set_uniform(std::string const& name, glm::mat4 const& value) const;

		void compile_and_link();
		void use() const;
		void unuse() const;

		unsigned int get_address() const;
		
	private:

		std::unique_ptr<Shader> _fs;
		std::unique_ptr<Shader> _gs;
		std::unique_ptr<Shader> _vs;

		unsigned int _address;
	};

	class ShaderProgramException : public std::runtime_error
	{
	public:
		ShaderProgramException(std::string error_message);
		
	private:
		std::string _message;
	};
} // namespace wt

#endif // GRAPHICS_SHADER_PROGRAM_H