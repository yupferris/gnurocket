cmake_minimum_required(VERSION 2.8)
project(rocket)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

add_subdirectory(sync)
add_subdirectory(example_sdl)
add_subdirectory(qt-editor)
