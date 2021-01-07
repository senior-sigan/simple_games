cmake_minimum_required(VERSION 3.13)
project(simple_games)

if (EMSCRIPTEN)
    set(PLATFORM Web)
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)
    set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

    add_definitions(-DPLATFORM_WEB)
    add_definitions(-DUSE_WASM)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --bind --memory-init-file 0 -s INITIAL_MEMORY=64MB -s SAFE_HEAP=1")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_GLFW=3 -s USE_WEBGL2=1 -s FORCE_FILESYSTEM=1")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s WASM=1")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s ASSERTIONS=1")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --use-preload-plugins")

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --bind --memory-init-file 0 -s INITIAL_MEMORY=64MB -s SAFE_HEAP=1")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s USE_GLFW=3 -s USE_WEBGL2=1 -s FORCE_FILESYSTEM=1")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s WASM=1")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s ASSERTIONS=1")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --use-preload-plugins")
    
    set(CMAKE_EXECUTABLE_SUFFIX ".js")
endif ()

set(BUILD_EXAMPLES OFF CACHE BOOL "Build the examples.")
set(BUILD_GAMES OFF CACHE BOOL "Build the example games.")
add_subdirectory(vendor/raylib-3.0.0)

add_subdirectory(raylib_test)
add_subdirectory(snake)
add_subdirectory(minesweeper)
