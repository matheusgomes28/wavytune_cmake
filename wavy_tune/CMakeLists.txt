find_package(Microsoft.GSL REQUIRED)
find_package(cxxopts REQUIRED)
find_package(kissfft REQUIRED)
find_package(miniaudio REQUIRED)
find_package(spdlog REQUIRED)

add_executable(wavy_tune)

target_sources(wavy_tune PRIVATE
  audio.cpp
  main.cpp
  window.cpp)

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
  spdlog::spdlog
  wavytune::analysis
  wavytune::graphics)
