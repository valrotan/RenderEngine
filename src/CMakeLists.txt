set(CMAKE_VERBOSE_MAKEFILE ON)

cmake_minimum_required(VERSION 3.5)

project(RenderEngine LANGUAGES C)

file( GLOB MATH_SOURCES src/math/*.c )
file( GLOB RENDERER_SOURCES src/renderer/*.c )
file( GLOB VISUALIZER_SOURCES src/visualizer/*.c )
file( GLOB SRC_SOURCES
	${MATH_SOURCES}
	${RENDERER_SOURCES}
	${VISUALIZER_SOURCES}
	)

file( GLOB TEST_SOURCES tests/*.c )

add_executable(engineRun src/main.c ${SRC_SOURCES})
add_executable(engineTest ${TEST_SOURCES} ${SRC_SOURCES})
#add_executable(gtkTest gtk.c)


find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
message( STATUS "GTK3_FOUND=${GTK3_FOUND}" )
include_directories(${GTK3_INCLUDE_DIRS})
target_link_directories(engineRun PRIVATE ${GTK3_LIBRARY_DIRS})
target_link_libraries(engineRun ${GTK3_LIBRARIES})
target_link_directories(engineTest PRIVATE ${GTK3_LIBRARY_DIRS})
target_link_libraries(engineTest ${GTK3_LIBRARIES})
#target_link_directories(gtkTest PRIVATE ${GTK3_LIBRARY_DIRS})
#target_link_libraries(gtkTest ${GTK3_LIBRARIES})
