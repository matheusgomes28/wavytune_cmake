find_package(glew REQUIRED)
find_package(glm REQUIRED)
find_package(glfw REQUIRED)
find_package(Microsoft.GSL REQUIRED)

add_library(graphics
	Camera.cpp
	Shader.cpp
	window.cpp
	ShaderProgram.cpp)

target_include_directories(graphics PUBLIC include)

target_link_libraries(graphics PUBLIC 
	data_structures
	glew::glew
	glm::glm
	glfw::glfw)

target_link_libraries(graphics PRIVATE
	Microsoft.GSL::GSL)