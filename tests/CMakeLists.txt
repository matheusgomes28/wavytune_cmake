find_package(GTest REQUIRED)

add_executable(matrix_test matrix_test.cpp)
target_link_libraries(matrix_test GTest::GTest fourier)
add_test(NAME matrices_unit_test COMMAND matrix_test)