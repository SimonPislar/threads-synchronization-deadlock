cmake_minimum_required(VERSION 3.23)
project(higher_grade C)

set(CMAKE_C_STANDARD 17)

add_executable(higher_grade/src
                sthreads_test.c
                sthreads.c)
