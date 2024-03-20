cmake_minimum_required( VERSION 3.18 )

project( EnzymeAigenTest LANGUAGES CXX )

# Use C++20
set( CMAKE_CXX_STANDARD          20  )
set( CMAKE_CXX_STANDARD_REQUIRED ON  )
set( CMAKE_CXX_EXTENSIONS        OFF )

# Use system Eigen3
find_package( Eigen3 REQUIRED )

# Use local Enzyme: Enzyme depends on the current LLVM version, which we
# can't predict - needs to be rebuilt with the project
add_subdirectory( Enzyme/enzyme )

add_executable( main main.cc )
target_link_libraries( main PRIVATE ClangEnzymeFlags Eigen3::Eigen )
target_compile_features( main PRIVATE cxx_std_20 )
