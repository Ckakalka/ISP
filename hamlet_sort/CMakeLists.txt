cmake_minimum_required(VERSION 3.10)

project(
        Hamlet_sort
        VERSION 1.0
        LANGUAGES C
)

set(CMAKE_C_FLAGS "-Wall -Wextra")

add_subdirectory(src)
add_subdirectory(tests)

find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(DOXYGEN_USE_MATHJAX YES)
    doxygen_add_docs(
            docs
            ${PROJECT_SOURCE_DIR}/src
    )
endif()