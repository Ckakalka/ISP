#include <stdio.h>
#include <malloc.h>
#include "equation.h"

int main()
{
    double a = 0;
    double b = 2;
    double c = 0;
    double *x_1 = (double*)malloc(sizeof(double));
    //double *x_1 = NULL;
    double *x_2 = (double*)malloc(sizeof(double));

    int roots_count = solve_quadratic_eq(a, b, c, x_1, x_2);

    if (roots_count != -1)
    {
        printf("Count of roots: %i\n x_1 = %f\n x_2 = %f\n", roots_count, *x_1, *x_2);
        printf("is_zero(x_1): %i\n", is_zero(*x_1));
    }
    else
        printf("Exception\n");


    free(x_1);
    free(x_2);

    return 0;
}

