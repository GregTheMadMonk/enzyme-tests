cmake_minimum_required( VERSION 3.18 )

project( EnzymeAigenTest LANGUAGES CXX )

# Use C++20
set( CMAKE_CXX_STANDARD          20  )
set( CMAKE_CXX_STANDARD_REQUIRED ON  )
set( CMAKE_CXX_EXTENSIONS        OFF )

add_subdirectory( Enzyme/enzyme )

add_executable( main main.cc )
target_link_libraries( main PRIVATE ClangEnzymeFlags )
target_compile_features( main PRIVATE cxx_std_20 )
