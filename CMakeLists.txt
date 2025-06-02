cmake_minimum_required(VERSION 3.16..4.0)

project(bmp C)

add_library(bmp STATIC lib/bmp.c lib/bmp.h)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_executable(demo src/main.c)
target_link_libraries(demo bmp)
target_include_directories(demo PUBLIC "lib")
