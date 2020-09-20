#include <math.h>
#include <errno.h>
#include <float.h>
#include "equation.h"
#include <assert.h>

/**
 * Function for solving a quadratic equation ax^2 + bx + c = 0.
 * @param a Leading coefficient
 * @param b Average coefficient
 * @param c Free term
 * @param x_1 Pointer to the first root (not NULL)
 * @param x_2 Pointer to the second root (not NULL)
 * @return Returns the number of roots on success. A value of 3 means an infinite number of roots.
 * @return A value of -1 indicates that an error occurred, the value of the errno variable in EDOM is also set.
 */
int solve_quadratic_eq(double a, double b, double c, double *x_1, double *x_2)
{
    assert(isfinite(a));
    assert(isfinite(b));
    assert(isfinite(c));

    if (is_zero(a))
        return solve_linear_eq(b, c, x_1);
    else
    {
        if (x_1 == NULL || x_2 == NULL)
        {
            errno = EDOM;
            return EXCEPTION;
        }

        errno = 0;

        // the discriminant
        *x_2 = b * b - 4 * a * c;

        if(!isfinite(*x_2))
        {
            errno = EDOM;
            return EXCEPTION;
        }

        if (is_zero(*x_2))
        {
            *x_1 = -b / (2 * a);
            return 1;
        }

        if (*x_2 < 0)
            return 0;

        // root of the discriminant
        *x_2 = sqrt(*x_2);

        *x_1 = (-b + *x_2) / (2 * a);
        *x_2 = (-b - *x_2) / (2 * a);
        return 2;
    }
}

/**
 * Function for solving a linear equation ax + b = 0.
 * @param a Coefficient of the unknown
 * @param b Free term
 * @param x Pointer to the root (not NULL)
 * @return Returns the number of roots on success. A value of 3 means an infinite number of roots.
 * @return A value of -1 indicates that an error occurred, the value of the errno variable in EDOM is also set.
 */
int solve_linear_eq(double a, double b, double *x)
{
    assert(isfinite(a));
    assert(isfinite(b));

    errno = 0;
    if (is_zero(a))
    {
        if (is_zero(b))
            return INFINITE_ROOTS;
        else
            return 0;
    }
    else
    {
        if (x == NULL)
        {
            errno = EDOM;
            return EXCEPTION;
        }

        *x = - b / a;

        if(!isfinite(*x))
        {
            errno = EDOM;
            return EXCEPTION;
        }

        return 1;
    }
}

/**
 * Checking a floating-point number for zero
 * @param a Number to check
 * @return Returns 1 if the number is zero, otherwise 0.
 */
int is_zero(double a)
{
    return fabs(a) <= EPSILON;
}
