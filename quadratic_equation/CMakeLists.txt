cmake_minimum_required(VERSION 3.15)
project(Quadratic_equation C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(Quadratic_equation
        equation.c
        equation.h
        main.c
        tests.c
        tests.h)
