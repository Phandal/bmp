cmake_minimum_required(VERSION 3.16..4.0)

project(bmp C)

add_library(bmp STATIC lib/bmp.c lib/bmp.h)
target_include_directories(bmp PUBLIC "lib")

option(PIXEL_PRINT "print read pixel data" OFF)
option(BMP_BUILD_DEMO "build the demo showing the features" ON)

if (BMP_BUILD_DEMO)
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
  add_executable(demo src/main.c)
  target_link_libraries(demo bmp)
  target_include_directories(demo PUBLIC "lib")
  if (PIXEL_PRINT)
      target_compile_definitions(demo PUBLIC PIXEL_PRINT)
  endif()
endif()
