cmake_minimum_required(VERSION 3.17)
project(simple_games)

set(BUILD_EXAMPLES OFF CACHE BOOL "Build the examples.")
set(BUILD_GAMES OFF CACHE BOOL "Build the example games.")
add_subdirectory(vendor/raylib-3.0.0)

add_subdirectory(raylib_test)
add_subdirectory(snake)