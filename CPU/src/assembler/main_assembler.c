/**
 * @file main_assembler.c
 * Console program for file assembly
 */

#include "assembler.h"

#include <stdio.h>

int main(int argc, char* argv[])
{
    if(argc <= 2) {
        printf("Error: Not enough arguments. Specify the path to the input and source file\n");
        return 0;
    }
    if(argc != 3) {
        printf("Error: Too many arguments. Specify the path to the input and source file\n");
        return 0;
    }

    size_t line_number = 1;
    int rc = assemble(argv[1], argv[2], &line_number);
    if(rc != OK)
        printf("Failure: error code: %i; breakpoint line: %u\n", rc, line_number);
    else
        printf("Assembling completed\n");
    return 0;
}

