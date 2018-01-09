cmake_minimum_required(VERSION 3.8)
project(computer_science_exercise)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES malloclab/mm.c
        malloclab/clock.c
        malloclab/config.h
        malloclab/fcyc.c
        malloclab/fsecs.c
        malloclab/fsecs.h
        malloclab/ftimer.c
        malloclab/ftimer.h
        malloclab/mdriver.c
        malloclab/memlib.c
        malloclab/memlib.h
        malloclab/mm.h)
add_executable(computer_science_exercise ${SOURCE_FILES})