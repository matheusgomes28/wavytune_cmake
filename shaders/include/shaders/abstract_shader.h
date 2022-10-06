#ifndef SHADERS_ABSRACT_SHADER_H
#define SHADERS_ABSRACT_SHADER_H

// Includes from the project
#include "data_structures/bytes.h"

// Includes from the STD
#include <string>
#include <stdexcept>

class ShaderException : public std::runtime_error
{
public:
	ShaderException(std::string err_message);
	const char* what() const noexcept;

private:
	std::string message_;
};

class AbstractShader
{
public:

	// Expensive copy here
	AbstractShader(const wt::ByteArray<500>&);
	AbstractShader(wt::ByteArray<500>&&);

	bool compile();
	void flag_delete();
	unsigned int get_address() const;
	std::string get_compilation_message() const;

private:
	unsigned int address_;
	wt::ByteArray<500> data_;
	

	virtual int _get_shader_type() const = 0;
};

#endif //SHADERS_ABSRACT_SHADER_H
