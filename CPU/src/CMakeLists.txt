add_library(
        stack_types
        stack/stack_types.c
)

add_library(
        loader
        loader.c
)

add_library(
        vector_label
        vector_label.c
)
#################################################################################
add_library(
        assembler_lib
        assembler/assembler.c
)

add_executable(
        assembler
        assembler/main_assembler.c
)

target_link_libraries(assembler assembler_lib loader vector_label)
#################################################################################
add_library(
        processor_lib
        processor/processor.c
)

add_executable(
        processor
        processor/main_processor.c
)

target_link_libraries(processor processor_lib loader stack_types)
#################################################################################
