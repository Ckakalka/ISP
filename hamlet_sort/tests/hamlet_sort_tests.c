/**
 * @file hamlet_sort_tests.c
 * The file for hamlet_sort tests.
 */
#include "../../Unity/src/unity.h"
#include "hamlet_sort.h"

// TODO Tests
int comparator(const void *left, const void *right)
{
        if ((*(int *)left) > (*(int *) right))
                return 1;
        else if ((*(int *) left) == (*(int *) right))
                return 0;
        return -1;
}

void setUp(void)
{
}
void tearDown(void)
{
}

void test_sort()
{
        int arr[3];
        for(int i = 0; i < 3; ++i)
                arr[i] = 3 - i;

        quick_sort(arr, 3, sizeof(int), &comparator);

        for(int i = 0; i < 3; ++i)
                TEST_ASSERT_EQUAL_INT(i + 1, arr[i]);

}

int main(void) {
        UNITY_BEGIN();
        RUN_TEST(test_sort);
        return UNITY_END();
}


