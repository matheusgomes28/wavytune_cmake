add_library(fourier source/dft_operations.cpp
					source/fourier.cpp)

target_include_directories(fourier PUBLIC include)

target_link_libraries(fourier PRIVATE matrix)