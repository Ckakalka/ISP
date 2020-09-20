/*!
\file
Header file with description of functions for solving a quadratic equation
*/
#ifndef HW_1_EQUATION_H
#define HW_1_EQUATION_H

#define INFINITE_ROOTS 3
#define EXCEPTION -1
#define EPSILON 8 * DBL_MIN

int solve_linear_eq(double, double, double*);
int solve_quadratic_eq(double, double, double, double*, double*);
int is_zero(double a);

#endif //HW_1_EQUATION_H
