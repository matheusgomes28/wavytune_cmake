add_executable(wavy_tune main.cpp)

target_link_libraries(wavy_tune
 PRIVATE
  GLEW::GLEW
  glfw
  portaudio_static
  fourier
  shaders)
