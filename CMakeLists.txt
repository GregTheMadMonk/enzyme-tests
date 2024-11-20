cmake_minimum_required( VERSION 3.30 FATAL_ERROR )

set( CMAKE_CXX_FLAGS_BAK "${CMAKE_CXX_FLAGS}" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++" )
set(
    CMAKE_EXPERIMENTAL_CXX_IMPORT_STD
    # See for your CMake version
    "0e5b6991-d74f-4b3d-a41c-cf096e0b2508"
)

project(
    lmhfe
    DESCRIPTION "Different differentiable implementations of LMHFE"
    LANGUAGES C CXX
)

set( CMAKE_INTERPROCEDURAL_OPTIMIZATION ON )
set( CMAKE_CXX_MODULE_STD ON )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic" )

if( CMAKE_BUILD_TYPE STREQUAL "Debug" )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined" )
endif()

include( CPM.cmake/cmake/CPM.cmake )

# Enzyme
# Enzyme should not be built with libc++ unless LLVM was built with it too
set( CMAKE_CXX_FLAGS_BAK2 "${CMAKE_CXX_FLAGS}" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_BAK}" )
CPMAddPackage(
    GITHUB_REPOSITORY "EnzymeAD/Enzyme"
    GIT_TAG "main"
    SOURCE_SUBDIR "enzyme"
)
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_BAK2}" )

if( NOT TARGET BLAS::BLAS )
    find_package( BLAS REQUIRED )
    message( STATUS "Found BLAS libraries ${BLAS_LIBRARIES}" )
endif()

function( add_module_library LIB_NAME )
    string( REPLACE "." "/" LIB_DIR "${LIB_NAME}" )
    string( REPLACE "." "_" LIB_TAR "${LIB_NAME}" )

    file( GLOB_RECURSE LIB_CC "src/${LIB_DIR}/*.cc" )
    file( GLOB_RECURSE LIB_XX "src/${LIB_DIR}/*.xx" )

    set_source_files_properties( ${LIB_XX} PROPERTIES LANGUAGE CXX )

    add_library( "${LIB_NAME}" STATIC ${LIB_CC} )
    target_sources(
        "${LIB_NAME}" PUBLIC
        FILE_SET "${LIB_TAR}_mod"
        TYPE CXX_MODULES
        FILES ${LIB_XX}
    )

    target_compile_features( "${LIB_NAME}" PUBLIC cxx_std_26 )
endfunction()

add_module_library( utils )
add_module_library( math )
target_link_libraries( math PRIVATE utils BLAS::BLAS )
add_module_library( cfd )
target_link_libraries( cfd PRIVATE math utils )
add_module_library( enzyme )
target_link_libraries( enzyme PUBLIC LLDEnzymeFlags )
target_include_directories( enzyme PRIVATE ${Enzyme_SOURCE_DIR}/enzyme/include )

file( GLOB_RECURSE UNIT_TEST_CC "test/unit/*.cc" )
add_executable( selftest ${UNIT_TEST_CC} )
target_compile_features( selftest PRIVATE cxx_std_26 )
target_link_libraries( selftest PRIVATE cfd math utils enzyme )
