/**
 * @file struct_and_enums.c
 * Contains return codes and structures for project
 */
#ifndef CPU_STRUCTS_AND_ENUMS_H
#define CPU_STRUCTS_AND_ENUMS_H
#include "stack/stack_rc.h"
#include <winbase.h>

typedef struct FileMapping {
    HANDLE file;
    HANDLE mapping;
    size_t size;
    const char* program;
} FileMapping;

#define NUMBER_COMMANDS 22
#define COMMAND_SIZE 1
typedef enum commands {
    PUSH = 0,
    PUSH_REG,
    POP,
    POP_REG,
    ADD,
    SUB,
    MUL,
    DIV,
    SIN,
    COS,
    SQRT,
    JMP,
    JE,
    JNE,
    JA,
    JAE,
    JB,
    JBE,
    CALL,
    RET,
    STOP,
#undef OUT
    OUT = 21
} commands;

#define NUMBER_COMMAND_MNEMONICS 20
typedef enum command_mnemonics {
    PUSH_M = 0,
    POP_M,
    ADD_M,
    SUB_M,
    MUL_M,
    DIV_M,
    SIN_M,
    COS_M,
    SQRT_M,
    JMP_M,
    JE_M,
    JNE_M,
    JA_M,
    JAE_M,
    JB_M,
    JBE_M,
    CALL_M,
    RET_M,
    STOP_M,
    OUT_M = 19
} command_mnemonics;

typedef enum runtime_errors {
    DIVISION_BY_ZERO = 10,
    ROOT_OF_NEGATIVE_VALUE,
    PROGRAM_BOUNDARY_ERROR,
    UNRECOGNIZED_REGISTER,
    UNRECOGNIZED_COMMAND,
    INCORRECT_TERMINATION = 15
} runtime_errors;

typedef enum cpu_rc {
    JMP_WAS_MADE = 16,
    JMP_WAS_NOT_MADE = 17
} cpu_rc;

typedef enum assembler_rc {
    END_OF_FILE = 18,
    EXPECTED_ARGUMENT,
    INVALID_ARGUMENT,
    ERROR_FILE_OPEN,
    ERROR_FILE_CLOSE,
    ERROR_FILE_WRITE,
    LABEL_ALREADY_EXIST,
    LABEL_NOT_FOUND = 25
} assembler_rc;

#define NUMBER_REGISTERS 4
#define REGISTER_SIZE 1
typedef enum registers {
    AX,
    BX,
    CX,
    DX
} registers;

#endif //CPU_STRUCTS_AND_ENUMS_H
