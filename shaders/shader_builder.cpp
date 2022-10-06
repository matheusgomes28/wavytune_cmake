#include "shaders/shader_builder.h"

ShaderBuilder::ShaderBuilder()
	: shader_program_{std::make_unique<ShaderProgram>()}
{
}

ShaderBuilder& ShaderBuilder::set_fragment_shader(const wt::ByteArray<500>& data)
{
	if (!shader_program_)
	{
		// TODO : shader_program_ is no longer 
		// TODO : valid
		throw ShaderBuilderException();
	}

	shader_program_->set_fragment_shader(
		std::make_unique<FragmentShader>(data)
	);
	return *this;
}

ShaderBuilder& ShaderBuilder::set_fragment_shader(wt::ByteArray<500>&& data)
{
	if (!shader_program_)
	{
		// TODO : shader_program_ is no longer 
		// TODO : valid
		throw ShaderBuilderException();
	}

	shader_program_->set_fragment_shader(
		std::make_unique<FragmentShader>(std::move(data))
	);
	return *this;
}

ShaderBuilder& ShaderBuilder::set_geometry_shader(const wt::ByteArray<500>& data)
{
	if (!shader_program_)
	{
		// TODO : shader_program_ is no longer
		// TODO : valid
		throw ShaderBuilderException();
	}

	shader_program_->set_geometry_shader(
		std::make_unique<GeometryShader>(data)
	);
	return *this;
}

ShaderBuilder& ShaderBuilder::set_geometry_shader(wt::ByteArray<500>&& data)
{
	if (!shader_program_)
	{
		// TODO : shader_program_ is no longer
		// TODO : valid
		throw ShaderBuilderException();
	}

	shader_program_->set_geometry_shader(
		std::make_unique<GeometryShader>(std::move(data))
	);
	return *this;
}


ShaderBuilder& ShaderBuilder::set_vertex_shader(const wt::ByteArray<500>& data)
{
	if (!shader_program_)
	{
		// TODO : shader_program_ is no longer
		// TODO : valid
		throw ShaderBuilderException();
	}

	shader_program_->set_vertex_shader(
		std::make_unique<VertexShader>(data)
	);
	return *this;
}

ShaderBuilder& ShaderBuilder::set_vertex_shader(wt::ByteArray<500>&& data)
{
	if (!shader_program_)
	{
		// TODO : shader_program_ is no longer
		// TODO : valid
		throw ShaderBuilderException();
	}

	shader_program_->set_vertex_shader(
		std::make_unique<VertexShader>(std::move(data))
	);
	return *this;
}

ShaderBuilder& ShaderBuilder::set_fragment_shader(std::unique_ptr<FragmentShader> fs)
{
	if (!shader_program_)
	{
		throw ShaderBuilderException();
	}
	
	shader_program_->set_fragment_shader(std::move(fs));
	return *this;
}

ShaderBuilder& ShaderBuilder::set_geometry_shader(std::unique_ptr<GeometryShader> gs)
{
	if (!shader_program_)
	{
		throw ShaderBuilderException();
	}
	
	shader_program_->set_geometry_shader(std::move(gs));
	return *this;
}

ShaderBuilder& ShaderBuilder::set_vertex_shader(std::unique_ptr<VertexShader> vs)
{
	if (!shader_program_)
	{
		throw ShaderBuilderException();
	}
	
	shader_program_->set_vertex_shader(std::move(vs));
	return *this;
}


std::unique_ptr<ShaderProgram> ShaderBuilder::build()
{
	if (!shader_program_)
	{
		throw ShaderBuilderException();
	}

	return std::move(shader_program_);
}