set(CMAKE_VERBOSE_MAKEFILE ON)

cmake_minimum_required(VERSION 3.5)

project(fullEngine LANGUAGES C)

add_executable(engineRun src/main.c)

add_executable(engineTest tests/tests.c)
