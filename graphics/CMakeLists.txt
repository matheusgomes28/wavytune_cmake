find_package(GLEW REQUIRED)
find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)
find_package(Microsoft.GSL REQUIRED)

add_library(graphics
	Camera.cpp
	window.cpp
  concrete_renderer.cpp
  draw_buffer.cpp)

target_include_directories(graphics PUBLIC include)

target_link_libraries(graphics
 PUBLIC 
  GLEW::GLEW
  glfw
  glm::glm
  wavytune::shaders)

target_link_libraries(graphics PRIVATE
	Microsoft.GSL::GSL)

add_library(wavytune::graphics ALIAS graphics)
