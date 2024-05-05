cmake_minimum_required( VERSION 3.28 )

project( tnl-mesh-test LANGUAGES CXX )

include( CPM.cmake/cmake/CPM.cmake )

CPMAddPackage(
    GITHUB_REPOSITORY "EnzymeAD/Enzyme"
    GIT_TAG "main"
    SOURCE_SUBDIR "enzyme"
)
CPMAddPackage( "gl:tnl-project/tnl#main" )
CPMAddPackage( "gh:madler/zlib#master" )

execute_process(
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/data.vtu ${CMAKE_CURRENT_BINARY_DIR}/data.vtu
)

add_executable( tnl-mesh-test main.cc )
target_link_libraries(
    tnl-mesh-test PRIVATE
    ClangEnzymeFlags TNL::TNL tinyxml2::tinyxml2 zlib
)
target_compile_features( tnl-mesh-test PRIVATE cxx_std_20 )
