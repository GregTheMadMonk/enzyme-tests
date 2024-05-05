cmake_minimum_required( VERSION 3.28 )

project( enzyme-tests LANGUAGES CXX )

file( GLOB TESTS "tests/*" )

foreach( TEST_DIR ${TESTS} )
    add_subdirectory( ${TEST_DIR} )
endforeach()
