find_package(glad REQUIRED)

add_executable(wavy_tune main.cpp)

target_link_libraries(wavy_tune
 PRIVATE
  glad::glad
  glfw::glfw
  portaudio_static
  shaders)
