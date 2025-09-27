find_package(Microsoft.GSL REQUIRED)
find_package(kissfft REQUIRED)

add_library(analysis)

target_sources(analysis PRIVATE analysis.cpp)

target_include_directories(analysis PUBLIC include)

target_link_libraries(analysis PRIVATE
    Microsoft.GSL::GSL
    kissfft::kissfft)

add_library(wavytune::analysis ALIAS analysis)
