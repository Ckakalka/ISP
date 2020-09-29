include_directories(../src)

include_directories(.)

add_executable(tests
        hamlet_sort_tests.c
        ../src/hamlet_sort.c
        ../src/hamlet_sort.h
        # unity test sources
        ../../Unity/src/unity.c
        )
