cmake_minimum_required(VERSION 3.10)
project(wavy_tune VERSION 0.1 LANGUAGES CXX)

find_package(GLEW REQUIRED)

add_subdirectory(data_structures)
add_subdirectory(fourier)
add_subdirectory(graphics)
add_subdirectory(matrix)

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
    data_structures
    fourier
    graphics
    matrix)
