cmake_minimum_required( VERSION 3.28 )

project( tnl-mesh-test LANGUAGES CXX )

add_subdirectory( 3rdparty/Enzyme/enzyme )
add_subdirectory( 3rdparty/tnl )

find_package( ZLIB REQUIRED )

execute_process(
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/data.vtu ${CMAKE_BINARY_DIR}/data.vtu
)

add_executable( tnl-mesh-test main.cc )
target_link_libraries(
    tnl-mesh-test PRIVATE
    ClangEnzymeFlags TNL::TNL tinyxml2::tinyxml2 ZLIB::ZLIB
)
target_compile_features( tnl-mesh-test PRIVATE cxx_std_20 )
