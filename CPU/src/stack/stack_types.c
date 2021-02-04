/**
 * @file stack_types.c
 * Defines the supported stack types and connects implementations of these types of functions
 * For each type, there must be a function void TEMPLATE(print_log, T)(FILE* file, const T* value_ptr)
 */
#include "stack_types.h"

#include <stdio.h>

#undef T
#define T elem_cpu_t
void TEMPLATE(print_log, T)(FILE* file, const T* value_ptr)
{
    fprintf(file, "%.3f", *value_ptr);
}
#include "stack_templates.c"

#undef T
#define T address_t
void TEMPLATE(print_log, T)(FILE* file, const T* value_ptr)
{
    fprintf(file, "%u", *value_ptr);
}
#include "stack_templates.c"

#undef T
#define T TEMPLATE(stack, elem_cpu_t)
void TEMPLATE(print_log, T)(FILE* file, const T* value_ptr)
{
    fprintf(file, "name = %s; size = %u; capacity = %u\n",
            value_ptr->name, value_ptr->size, value_ptr->capacity);
}
#include "stack_templates.c"
