add_library(
        hamlet_sort
        hamlet_sort.c
)

add_executable(
        app
        main.c
)

target_link_libraries(app hamlet_sort)

#[[add_executable(
        tests
        tests.c
)

target_link_libraries(tests quadratic)
]]