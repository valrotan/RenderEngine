set(CMAKE_VERBOSE_MAKEFILE ON)

cmake_minimum_required( VERSION 3.5 )
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

project( RenderEngine LANGUAGES C )

add_executable( RenderEngine xlib_stream.c )

find_package(X11 REQUIRED)
include_directories(${X11_INCLUDE_DIR})
target_link_libraries( RenderEngine ${X11_LIBRARIES} )

message(STATUS "X11_FOUND = ${X11_FOUND}")
