cmake_minimum_required( VERSION 3.18 )

project( MultiFile LANGUAGES CXX )

include( CPM.cmake/cmake/CPM.cmake )

CPMAddPackage(
    GITHUB_REPOSITORY "EnzymeAD/Enzyme"
    GIT_TAG "main"
    SOURCE_SUBDIR "enzyme"
)
# This doesn't get automaticlly added with LLDEnzymeFlags
include_directories( "${Enzyme_SOURCE_DIR}/enzyme/include" )

add_executable( out1 src/main.cc src/lib1.cc )
add_executable( out2 src/main.cc src/lib2.cc )
target_link_libraries( out1 PRIVATE LLDEnzymeFlags )
target_link_libraries( out2 PRIVATE LLDEnzymeFlags )
