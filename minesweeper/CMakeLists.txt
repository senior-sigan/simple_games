cmake_minimum_required(VERSION 3.13)
project(minesweeper C)

set(CMAKE_C_STANDARD 11)

add_executable(${PROJECT_NAME} main.c)

target_link_libraries(${PROJECT_NAME} raylib)

#if (EMSCRIPTEN)
#    file(COPY index.html DESTINATION ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
#endif ()