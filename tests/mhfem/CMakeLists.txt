cmake_minimum_required( VERSION 3.28 ) # C++20 modules baby!

set( CMAKE_CXX_STANDARD 23 )
include( CppStandardLibraryModule/cmake/EnableStandardLibraryModule.cmake )

file( GLOB_RECURSE MHFEM_XX *.xx )
file( GLOB_RECURSE MHFEM_CC *.cc )
set_source_files_properties( ${MHFEM_XX} PROPERTIES LANGUAGE CXX )
add_executable( mhfem ${MHFEM_CC} )

target_sources(
    mhfem PUBLIC
    FILE_SET mhfem_src
    TYPE CXX_MODULES
    FILES ${MHFEM_XX}
)
target_link_libraries( mhfem PRIVATE ClangEnzymeFlags )
set_target_properties(
    mhfem PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}
)
target_compile_features( mhfem PRIVATE cxx_std_23 )
