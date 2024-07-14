cmake_minimum_required( VERSION 3.18 )

project( avx-tests LANGUAGES CXX )

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic" )

include( CPM.cmake/cmake/CPM.cmake )

CPMAddPackage(
    NAME Enzyme
    URL "https://github.com/EnzymeAD/Enzyme/archive/refs/heads/main.zip"
    SOURCE_SUBDIR "enzyme"
)

add_executable( test-avx2 main.cc )
target_compile_options(
    test-avx2 PRIVATE
    "-march=x86-64-v3" "-mllvm" "-force-vector-width=8"
)
target_compile_features( test-avx2 PRIVATE cxx_std_17 )
target_link_libraries( test-avx2 PRIVATE ClangEnzymeFlags )

add_executable( test main.cc )
target_compile_options(
    test PRIVATE
    "-DNO_INTRINSICS=1" "-march=x86-64-v3" "-mllvm" "-force-vector-width=8"
)
target_compile_features( test PRIVATE cxx_std_17 )
target_link_libraries( test PRIVATE ClangEnzymeFlags )
