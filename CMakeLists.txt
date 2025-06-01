cmake_minimum_required(VERSION 3.16..4.0)

project(bmp C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_executable(bmp src/main.c)
add_executable(bmp2 src/main_new.c)
