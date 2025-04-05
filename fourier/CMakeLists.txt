add_library(fourier src/dft_operations.cpp
					src/fourier.cpp)

target_include_directories(fourier PUBLIC include)

target_link_libraries(fourier PUBLIC matrix)
