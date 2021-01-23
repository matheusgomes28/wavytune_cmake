find_package(glm REQUIRED)
find_package(glfw REQUIRED)

add_library(graphics Camera.cpp window.cpp)

target_include_directories(graphics PUBLIC include)

target_link_libraries(graphics PUBLIC 
	glm::glm
	glfw::glfw)