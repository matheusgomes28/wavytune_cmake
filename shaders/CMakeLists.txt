add_library(shaders
  abstract_shader.cpp
  shader_builder.cpp
  shader_program.cpp)

target_include_directories(shaders PUBLIC include)

# Need to see why these things are leaking
target_link_libraries(shaders
 PUBLIC
  GLEW::GLEW
  glm::glm
  data_structures)