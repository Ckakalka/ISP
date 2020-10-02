#include <stdio.h>
#include "hamlet_sort.h"



int main(int argc, char *argv[])
{
        if (argc != 2) {
                printf("Provide only file path");
                return 0;
        }
        hamlet_sort(argv[1]);
        return 0;
}

