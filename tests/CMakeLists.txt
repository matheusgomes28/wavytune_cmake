find_package(GTest REQUIRED)


#
# Create custom target and the library
# containing the main for gtests, which
# must be added as a dependency to every
# test exeutable
add_custom_target(all-tests)
add_library(main_unit_test main_unit_test.cpp)
target_link_libraries(main_unit_test PUBLIC GTest::GTest)

#
# All the tests should be added under
# the all-tests target for the pipeline
add_executable(matrix_test matrix_test.cpp)
add_dependencies(all-tests matrix_test)
add_test(unit-tests-matrix_tests matrix_test)
target_link_libraries(matrix_test fourier main_unit_test)
