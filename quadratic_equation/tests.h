#ifndef HW_1_TESTS_H
#define HW_1_TESTS_H

#define TESTS_COUNT 11

void run_all_tests(void);

static void is_zero_test_1(void);

static void simple_linear_eq_test_1(void);
static void simple_linear_eq_test_2(void);
static void no_solution_linear_eq_test(void);
static void infinity_roots_linear_eq_test(void);
static void null_ptr_linear_eq_test(void);

static void simple_quadratic_eq_test(void);
static void one_root_quadratic_eq_test(void);
static void negative_discriminant_quadratic_eq_test(void);
static void null_ptr_quadratic_eq_test(void);
static void quadratic_to_linear_eq_test(void);


#endif //HW_1_TESTS_H
