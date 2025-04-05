find_package(miniaudio REQUIRED)

add_executable(wavy_tune main.cpp)

target_link_libraries(wavy_tune
 PRIVATE
  GLEW::GLEW
  miniaudio::miniaudio
  glfw
  fourier
  shaders)
