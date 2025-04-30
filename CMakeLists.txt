cmake_minimum_required(VERSION 3.16)
project(wavy_tune VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)

find_package(glew REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

if (WIN32)
    set(MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

add_subdirectory(analysis)
add_subdirectory(fourier)
add_subdirectory(graphics)
add_subdirectory(matrix)
add_subdirectory(shaders)
add_subdirectory(wavy_tune)

# Only build if the options were given
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    add_subdirectory(tests)
endif()

add_executable(wavy_main source/main.cpp)
target_link_libraries(wavy_main PRIVATE
    GLEW::GLEW
    fourier
    graphics
    matrix)
