find_package(miniaudio REQUIRED)
find_package(cxxopts REQUIRED)

add_executable(wavy_tune main.cpp)

target_link_libraries(wavy_tune
 PRIVATE
  GLEW::GLEW
  cxxopts::cxxopts
  miniaudio::miniaudio
  glfw
  fourier
  shaders)
