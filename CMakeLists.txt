set(CMAKE_VERBOSE_MAKEFILE ON)

cmake_minimum_required( VERSION 3.5 )

project( Test LANGUAGES C )

find_package(X11 REQUIRED)

message(STATUS "X11_FOUND = ${X11_FOUND}")
message(STATUS "X11_INCLUDE_DIR = ${X11_INCLUDE_DIR}")
message(STATUS "X11_LIBRARIES = ${X11_LIBRARIES}")

include_directories(${X11_INCLUDE_DIR})

add_executable( Test main.c )

target_link_libraries( Test ${X11_LIBRARIES} )
