/**
 * @file main_processor.c
 * Console program for running programs
 */
#include "processor.h"

#include <stdio.h>

int main(int argc, char* argv[])
{
    if(argc <= 1) {
        printf("Error: Not enough arguments. Specify the path to the input file\n");
        return 0;
    }
    if(argc != 2) {
        printf("Error: Too many arguments. Specify the path to the input file\n");
        return 0;
    }

    int rc = run(argv[1]);
    if(rc != OK)
        printf("Failure: error code: %i\n", rc);
    else
        printf("The execution is completed\n");
    return 0;
}
