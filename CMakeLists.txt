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
