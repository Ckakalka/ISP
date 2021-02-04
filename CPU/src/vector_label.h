/**
 * @file vector_label.h
 * Contains function headers of the Vector and definition of the Vector
 */
#ifndef CPU_VECTOR_LABEL_H
#define CPU_VECTOR_LABEL_H

#include <stddef.h>

#include "stack/stack_rc.h"

#define INDEX_OUT_OF_RANGE 129
#define VECTOR_IS_NULL STACK_IS_NULL
#define VECTOR_IS_EMPTY STACK_IS_EMPTY
#define VECTOR_OVERFLOW STACK_OVERFLOW
#define VECTOR_ALREADY_DELETED STACK_ALREADY_DELETED


typedef struct Label {
    const char* name;
    size_t address;
} Label;

typedef struct Vector {
    size_t size;
    size_t capacity;
    Label* data;
} Vector;


int vector_create(Vector** vector_ref);
int vector_destroy(Vector** vector);
int vector_get(Vector* vector, Label* label, size_t index);
int vector_pop(Vector* vector);
int vector_push(Vector* vector, const Label* label_ref);


static int vector_resize(Vector* vector, size_t new_size);
#endif //CPU_VECTOR_LABEL_H
