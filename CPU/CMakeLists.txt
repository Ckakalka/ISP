cmake_minimum_required(VERSION 3.10)

project(
        CPU
        VERSION 1.0
        LANGUAGES C
)

set(CMAKE_C_FLAGS "-Wall -Wextra")

add_subdirectory(src)
