add_library(fourier include/fourier/dft.h
					include/fourier/dft_operations.h
					include/fourier/matrix.h
					include/fourier/matrix_exception.h
					include/fourier/matrix_row.h
					include/fourier/matrix_traits.h
					include/fourier/sft.h
					source/dft_operations.cpp
					source/fourier.cpp)
target_include_directories(fourier PUBLIC include)