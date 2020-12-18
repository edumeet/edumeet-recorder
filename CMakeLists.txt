cmake_minimum_required(VERSION 3.14)
project(edumeet-recorder)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

option(BUILD_TESTS "Build test programs" OFF)
if (BUILD_TESTS)
    enable_testing()
endif (BUILD_TESTS)

add_subdirectory(src)
install (DIRECTORY ${CMAKE_BINARY_DIR}/bin DESTINATION bin)
