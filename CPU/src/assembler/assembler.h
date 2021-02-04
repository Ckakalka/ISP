/**
 * @file assembler.h
 * Contains function headers for file assembly
 */
#ifndef CPU_ASSEMBLER_H
#define CPU_ASSEMBLER_H

#include "../structs_and_enums.h"


int assemble(const char* input, const char* output, size_t* line_number);

static void read_word(const FileMapping* file_mapping, size_t* offset, size_t* size, size_t* line_number);
static int str_is_equal_i(const char* first, const char* second);
static int str_is_equal(const char* first, const char* second);

#endif //CPU_ASSEMBLER_H
