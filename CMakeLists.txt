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

file( GLOB TEST_DIRS tests/* )
foreach( TEST_DIR ${TEST_DIRS} )
    if( EXISTS "${TEST_DIR}/CMakeLists.txt" )
        add_subdirectory( "${TEST_DIR}" )
    else()
        get_filename_component( TEST_NAME "${TEST_DIR}" NAME_WE )
        file( GLOB_RECURSE TEST_SRC "${TEST_DIR}/*.cc" )
        add_executable( ${TEST_NAME} "${TEST_SRC}" )
        target_include_directories( ${TEST_NAME} PRIVATE "${TEST_SRC}" )
        target_link_libraries(
            ${TEST_NAME} PRIVATE ClangEnzymeFlags Eigen3::Eigen
        )
        target_compile_features( ${TEST_NAME} PRIVATE cxx_std_20 )
    endif()
endforeach()
