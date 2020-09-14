#include <math.h>
#include <errno.h>
#include <float.h>
#include "equation.h"

/**
 * Функция для решения квадратного уравнения ax^2 + bx + c = 0.
 * @param a Старший коэффициент
 * @param b Средний коэффициент
 * @param c Свободный член
 * @param x_1 Указатель на первый корень (не NULL)
 * @param x_2 Указатель на второй корень (не NULL)
 * @return При успешном выполнении возвращает количество корней. Значение 3 означает бесконечное количество корней.
 * @return Значение -1 означает возникновение ошибки: обращение к нулевому указателю, также устанавливается значение переменной errno в EDOM.
 */
int solve_quadratic_eq(double a, double b, double c, double *x_1, double *x_2)
{
    if (is_zero(a))
        return solve_linear_eq(b, c, x_1);
    else
    {
        if (x_1 == NULL || x_2 == NULL)
        {
            errno = EDOM;
            return NULL_PTR_EXC;
        }

        errno = 0;

        // дискриминант
        *x_2 = b * b - 4 * a * c;

        if (is_zero(*x_2))
        {
            *x_1 = -b / (2 * a);
            return 1;
        }

        if (*x_2 < 0)
            return 0;

        // корень из дискриминанта
        *x_2 = sqrt(*x_2);

        *x_1 = (-b + *x_2) / (2 * a);
        *x_2 = (-b - *x_2) / (2 * a);
        return 2;
    }
}

/**
 * Функция для решения линейного уравнения ax + b = 0.
 * @param a Коэффициент перед неизвестным
 * @param b Свободный член
 * @param x Указатель на корень
 * @return При успешном возвращении возвращает количество корней. Значение 3 означает бесконечное количество корней.
 * @return Значение -1 означает возникновение ошибки: обращение к нулевому указателю, также устанавливается значение переменной errno в EDOM.
 */
int solve_linear_eq(double a, double b, double *x)
{
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
            return NULL_PTR_EXC;
        }

        *x = - b / a;
        return 1;
    }
}

/**
 * Проверка числа с плавающей точкой на равенство нулю
 * @param a Число, которое необходимо проверить
 * @return Возвращает 1 если число равно нулю, иначе 0.
 */
int is_zero(double a)
{
    return fabs(a) <= EPSILON;
}
