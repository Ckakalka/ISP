#include "tests.h"
#include <locale.h>

int main()
{
    setlocale(LC_ALL, "Russian");
    run_all_tests();

    return 0;
}

