find_package(Microsoft.GSL REQUIRED)
find_package(cxxopts REQUIRED)
find_package(kissfft REQUIRED)
find_package(miniaudio REQUIRED)

add_executable(wavy_tune main.cpp window.cpp)

target_link_libraries(wavy_tune
 PRIVATE
  GLEW::GLEW
  Microsoft.GSL::GSL
  cxxopts::cxxopts
  fourier
  glfw
  kissfft::kissfft
  miniaudio::miniaudio
  shaders
  wavytune::analysis
  wavytune::graphics)
