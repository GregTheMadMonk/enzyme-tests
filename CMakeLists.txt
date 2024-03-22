cmake_minimum_required( VERSION 3.18 )

project( EnzymeAigenTest LANGUAGES CXX )

# Use C++20
set( CMAKE_CXX_STANDARD          20  )
set( CMAKE_CXX_STANDARD_REQUIRED ON  )
set( CMAKE_CXX_EXTENSIONS        OFF )

# Be self-sufficient - use Eigen as a submodule!
add_subdirectory( eigen )

# Use local Enzyme: Enzyme depends on the current LLVM version, which we
# can't predict - needs to be rebuilt with the project
add_subdirectory( Enzyme/enzyme )

file( GLOB TESTS tests/*.cc )
foreach( TEST ${TESTS} )
    get_filename_component( TEST_NAME ${TEST} NAME_WE )
    add_executable( ${TEST_NAME} ${TEST} )
    target_link_libraries( ${TEST_NAME} PRIVATE ClangEnzymeFlags Eigen3::Eigen )
    target_compile_features( ${TEST_NAME} PRIVATE cxx_std_20 )
endforeach()
