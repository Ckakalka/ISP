#include "equation.h"
#include "tests.h"
#include "errno.h"
#include "stdio.h"
#include "float.h"

void run_all_tests(void)
{
    void (*tests[TESTS_COUNT])(void);

    tests[0]  = is_zero_test_1;
    tests[1]  = simple_linear_eq_test_1;
    tests[2]  = simple_linear_eq_test_2;
    tests[3]  = no_solution_linear_eq_test;
    tests[4]  = infinity_roots_linear_eq_test;
    tests[5]  = null_ptr_linear_eq_test;
    tests[6]  = simple_quadratic_eq_test;
    tests[7]  = one_root_quadratic_eq_test;
    tests[8] = negative_discriminant_quadratic_eq_test;
    tests[9] = null_ptr_quadratic_eq_test;
    tests[10] = quadratic_to_linear_eq_test;

    for(int i = 0; i < TESTS_COUNT; ++i)
        tests[i]();
}

static void is_zero_test_1(void)
{
    double a = 5 * DBL_MIN;
    if(is_zero(a))
        printf("is_zero_test_1 is successful\n");
    else
        printf("\nis_zero_test_1 failed\n\n");
}

static void simple_linear_eq_test_1(void)
{
    double a = 2;
    double b = 8;
    double x = 0;
    int roots_count = solve_linear_eq(a, b, &x);
    if(errno)
    {
        printf("\nsimple_linear_eq_test_1 failed: errno != 0.\n\n");
        return;
    }

    if(!(is_zero(x + 4) && roots_count == 1))
    {
        printf("\nsimple_linear_eq_test_1 failed: wrong result!\n"
               "Expected roots count: 1. Actual roots count: %d\n"
               "Expected x = -4. Actual x = %f\n\n", roots_count, x);
        return;
    }
    printf("simple_linear_eq_test_1 is successful\n");
}

static void simple_linear_eq_test_2(void)
{
    double a = 375;
    double b = 16;
    double x = 0;
    int roots_count = solve_linear_eq(a, b, &x);
    if(errno)
    {
        printf("\nsimple_linear_eq_test_2 failed: errno != 0.\n\n");
        return;
    }

    if(!(is_zero(x + 16.0 / 375) && roots_count == 1))
    {
        printf("\nsimple_linear_eq_test_2 failed: wrong result!\n"
               "Expected roots count: 1. Actual roots count: %d\n"
               "Expected x = -0.042(6). Actual x = %f\n\n", roots_count, x);
        return;
    }
    printf("simple_linear_eq_test_2 is successful\n");
}

static void no_solution_linear_eq_test(void)
{
    double a = 0;
    double b = 2;
    double x = 0;
    int roots_count = solve_linear_eq(a, b, &x);
    if(errno)
    {
        printf("\nno_solution_linear_eq_test failed: errno != 0.\n\n");
        return;
    }
    if(roots_count !=0)
    {
        printf("\nno_solution_linear_eq_test failed: wrong result!\n"
               "Expected roots count: 0. Actual roots count: %d\n\n", roots_count);
        return;
    }
    printf("no_solution_linear_eq_test is successful\n");
}

static void infinity_roots_linear_eq_test(void)
{
    double a = 0;
    double b = 0;
    double x = 0;
    int roots_count = solve_linear_eq(a, b, &x);
    if(errno)
    {
        printf("\ninfinity_roots_linear_eq_test != 0.\n\n");
        return;
    }
    if(roots_count != INFINITE_ROOTS)
    {
        printf("\ninfinity_roots_linear_eq_test failed: wrong result!\n"
               "Expected roots count: INFINITY. Actual roots count: %d\n\n", roots_count);
        return;
    }
    printf("infinity_roots_linear_eq_test is successful\n");
}

static void null_ptr_linear_eq_test(void)
{
    double a = 21;
    double b = 0;
    double *x = NULL;
    int roots_count = solve_linear_eq(a, b, x);
    if(errno && roots_count == EXCEPTION)
        printf("null_ptr_linear_eq_test is successful\n");
    else
        printf("\nnull_ptr_linear_eq_test failed: wrong result!\n"
               "Expected roots count: EXCEPTION. Actual roots count: %d\n"
               "Expected errno = EDOM(33). Actual errno = %d\n\n", roots_count, errno);
}

static void simple_quadratic_eq_test(void)
{
    double a = 2;
    double b = 5;
    double c = 3;
    double x_1 = 0;
    double x_2 = 0;

    int roots_count = solve_quadratic_eq(a, b, c, &x_1, &x_2);

    if(errno)
    {
        printf("\nsimple_quadratic_eq_test failed: errno != 0.\n\n");
        return;
    }
    if(!(roots_count == 2 && is_zero(x_1 + 1) && is_zero(x_2 + 1.5)))
    {
        printf("\nsimple_quadratic_eq_test failed: wrong result!\n"
               "Expected roots count: 2. Actual roots count: %d\n"
               "Expected x_1 = -1. Actual x = %f\n"
               "Expected x_2 = -1.5. Actual x =%f\n\n", roots_count, x_1, x_2);
        return;
    }
    printf("simple_quadratic_eq_test is successful\n");
}

static void one_root_quadratic_eq_test(void)
{
    double a = 1;
    double b = 8;
    double c = 16;
    double x_1 = 0;
    double x_2 = 0;

    int roots_count = solve_quadratic_eq(a, b, c, &x_1, &x_2);

    if(errno)
    {
        printf("\none_root_quadratic_eq_test failed: errno != 0.\n\n");
        return;
    }
    if(!(roots_count == 1 && is_zero(x_1 + 4)))
    {
        printf("\none_root_quadratic_eq_test failed: wrong result!\n"
               "Expected roots count: 1. Actual roots count: %d\n"
               "Expected x_1 = -4. Actual x = %f\n\n", roots_count, x_1);
        return;
    }
    printf("one_root_quadratic_eq_test is successful\n");
}

static void negative_discriminant_quadratic_eq_test(void)
{
    double a = 2;
    double b = 5;
    double c = 19;
    double x_1 = 0;
    double x_2 = 0;

    int roots_count = solve_quadratic_eq(a, b, c, &x_1, &x_2);

    if(errno)
    {
        printf("\nnegative_discriminant_quadratic_eq_test failed: errno != 0.\n\n");
        return;
    }
    if(roots_count)
    {
        printf("\nnegative_discriminant_quadratic_eq_test failed: wrong result!\n"
               "Expected roots count: 0. Actual roots count: %d\n", roots_count);
        return;
    }
    printf("negative_discriminant_quadratic_eq_test is successful\n");
}

static void null_ptr_quadratic_eq_test(void)
{
    double a = 2;
    double b = 5;
    double c = 19;
    double *x_1 = NULL;
    double *x_2 = NULL;

    int roots_count = solve_quadratic_eq(a, b, c, x_1, x_2);

    if(errno && roots_count == EXCEPTION)
        printf("null_ptr_quadratic_eq_test is successful\n");
    else
        printf("\nnull_ptr_quadratic_eq_test failed: wrong result!\n"
               "Expected roots count: EXCEPTION. Actual roots count: %d\n"
               "Expected errno = EDOM(33). Actual errno = %d\n\n", roots_count, errno);
}

static void quadratic_to_linear_eq_test(void)
{
    double a = 0;
    double b = 5;
    double c = 30;
    double x_1 = 0;
    double x_2 = 0;

    int roots_count = solve_quadratic_eq(a, b, c, &x_1, &x_2);

    if (errno)
    {
        printf("\nquadratic_to_linear_eq_test failed: errno != 0.\n\n");
        return;
    }
    if (roots_count != 1)
    {
        printf("\nquadratic_to_linear_eq_test failed: wrong result!\n"
               "Expected roots count: 1. Actual roots count: %d\n"
               "Expected x_1 = -6. Actual x_1 = %f\n\n", roots_count, x_1);
        return;
    }
    printf("quadratic_to_linear_eq_test is successful\n");
}




