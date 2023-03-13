find_package(glad REQUIRED)

add_library(shaders shader_program.cpp)

target_include_directories(shaders PUBLIC include)

# Need to see why these things are leaking
target_link_libraries(shaders
 PUBLIC
  glad::glad
  glm::glm
  data_structures)