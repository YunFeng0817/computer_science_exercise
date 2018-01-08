cmake_minimum_required(VERSION 3.8)
project(computer_science_exercise)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES malloclab/mm.c)
add_executable(computer_science_exercise ${SOURCE_FILES})