cmake_minimum_required( VERSION 3.28 )

project( avx-tests LANGUAGES CXX )

# Treat all *.xx files as C++ sources
file( GLOB_RECURSE XX_FILES "*.xx" )
set_source_files_properties( ${XX_FILES} PROPERTIES LANGUAGE CXX )

add_library( WAllExtra INTERFACE )
target_compile_options( WAllExtra INTERFACE -Wall -Wextra -pedantic )

include( CPM.cmake/cmake/CPM.cmake )

CPMAddPackage(
    NAME Enzyme
    URL "https://github.com/EnzymeAD/Enzyme/archive/refs/heads/main.zip"
    SOURCE_SUBDIR "enzyme"
)
set( BENCHMARK_ENABLE_GTEST_TESTS OFF )
CPMAddPackage(
    NAME "gbenchmark"
    URL "https://github.com/google/benchmark/archive/refs/heads/main.zip"
)

add_library( enzyme STATIC )
target_sources(
    enzyme PUBLIC
    FILE_SET enzyme_mod
    TYPE CXX_MODULES
    FILES src/enzyme/enzyme.xx
)
target_link_libraries( enzyme PUBLIC ClangEnzymeFlags )
target_compile_features( enzyme PUBLIC cxx_std_20 )

file( GLOB ALIGNED_XX "src/aligned/*.xx" )
add_library( aligned STATIC )
target_sources(
    aligned PUBLIC
    FILE_SET aligned_mod
    TYPE CXX_MODULES
    FILES ${ALIGNED_XX}
)
target_compile_features( aligned PUBLIC cxx_std_20 )

file( GLOB_RECURSE MAIN_XX "src/main/*.xx" )
add_library( main STATIC )
target_sources(
    main PUBLIC
    FILE_SET main_mod
    TYPE CXX_MODULES
    FILES ${MAIN_XX}
)
target_compile_features( main PUBLIC cxx_std_20 )
target_link_libraries(
    main
    PUBLIC WAllExtra
    PRIVATE enzyme aligned benchmark::benchmark
)
if( USE_AVX2 )
    target_compile_definitions( main PUBLIC USE_AVX2 )
    target_compile_options( main PUBLIC "-march=x86-64-v3" )
endif()
if( USE_OMP )
    target_compile_definitions( main PUBLIC USE_OMP )
    find_package( OpenMP REQUIRED )
    # OpenMP is invasive! It needs to be linked to all modules
    target_link_libraries( main PUBLIC OpenMP::OpenMP_CXX)
    target_link_libraries( aligned PUBLIC OpenMP::OpenMP_CXX)
    target_link_libraries( enzyme PUBLIC OpenMP::OpenMP_CXX)
endif()

add_executable( test main.cc )
target_compile_options(
    test PRIVATE
    "-mllvm" "-force-vector-width=8"
)
target_compile_features( test PRIVATE cxx_std_20 )
target_link_libraries(
    test PRIVATE
    aligned main enzyme
    benchmark::benchmark
)
