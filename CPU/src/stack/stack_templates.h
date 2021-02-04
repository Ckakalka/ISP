/**
 * @file stack_templates.h
 * Contains the function headers of the stack as well as the definition of the stack structure
 */
#ifdef T

#include <stddef.h>

#include "templates.h"

typedef struct TEMPLATE(stack, T) {
    size_t size;
    size_t capacity;
    char* name;
    T *data;
    // TODO Implement hash protector
    // unsigned long long hash_data;
    // unsigned long long hash_entries;
} TEMPLATE(stack, T);

int TEMPLATE(stack_create, T)(TEMPLATE(stack, T)** stack, char* name);
int TEMPLATE(stack_destroy, T)(TEMPLATE(stack, T)** stack);
int TEMPLATE(stack_destroy_ref, T)(TEMPLATE(stack, T)** stack);
int TEMPLATE(stack_destroy_data, T)(TEMPLATE(stack, T)** stack);
int TEMPLATE(stack_get_size, T)(TEMPLATE(stack, T)* stack, size_t* size);
int TEMPLATE(stack_get_capacity, T)(TEMPLATE(stack, T)* stack, size_t* capacity);
int TEMPLATE(stack_push, T)(TEMPLATE(stack, T)* stack, T elem);
int TEMPLATE(stack_ref_push, T)(TEMPLATE(stack, T)* stack, T* elem_ref);
int TEMPLATE(stack_pop, T)(TEMPLATE(stack, T)* stack, T* elem);
int TEMPLATE(stack_ref_pop, T)(TEMPLATE(stack, T)* stack, T** elem);
int TEMPLATE(stack_peek, T)(TEMPLATE(stack, T)* stack, T* elem);
int TEMPLATE(stack_ref_peek, T)(TEMPLATE(stack, T)* stack, T** elem);
int TEMPLATE(stack_clear, T)(TEMPLATE(stack, T)* stack);


static int TEMPLATE(stack_resize, T)(TEMPLATE(stack, T)* stack, size_t new_size);
static void TEMPLATE(dump, T)(char* function_name, int rc, TEMPLATE(stack, T)* stack);

#endif
