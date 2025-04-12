find_package(GLEW REQUIRED)
find_package(glm REQUIRED)
find_package(Microsoft.GSL)

add_library(shaders
  abstract_shader.cpp
  shader_builder.cpp
  shader_program.cpp)

target_include_directories(shaders PUBLIC include)

# Need to see why these things are leaking
target_link_libraries(shaders
 PUBLIC
  GLEW::GLEW
  Microsoft.GSL::GSL
  glm::glm)

add_library(wavytune::shaders ALIAS shaders)
