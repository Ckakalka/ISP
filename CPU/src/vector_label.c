/**
 * @file vector_label.c
 * Contains function implementations of the Vector
 */
#include "vector_label.h"

#include <stdint.h>
#include <stdlib.h>

#ifndef _ASSERT_H_
#include <assert.h>
#endif

#define INITIAL_CAPACITY 10
#define VECTOR_SIZE_MAX SIZE_MAX
#define RESIZE_FACTOR 2
#define POINTER_POISON 0xF

int vector_create(Vector** vector_ref)
{
    if(vector_ref == NULL)
        return ARGUMENT_IS_NULL;

    *vector_ref = (Vector*)calloc(1, sizeof(Vector));
    if((*vector_ref) == NULL)
        return MEMORY_ALLOCATION_ERROR;

    (*vector_ref)->data = (Label*)calloc(INITIAL_CAPACITY, sizeof(Label));
    if((*vector_ref)->data == NULL) {
        free(*vector_ref);
        return MEMORY_ALLOCATION_ERROR;
    }
    (*vector_ref)->size = 0;
    (*vector_ref)->capacity = INITIAL_CAPACITY;
    return OK;
}

int vector_destroy(Vector** vector)
{
    if(vector == NULL)
        return ARGUMENT_IS_NULL;
    if(*vector == (Vector*)POINTER_POISON)
        return VECTOR_ALREADY_DELETED;

    free((*vector)->data);
    (*vector)->data = (Label*)POINTER_POISON;

    free(*vector);
    *vector = (Vector*)POINTER_POISON;
    return OK;
}

int vector_get(Vector* vector, Label* label, size_t index)
{
    if(vector == NULL)
        return VECTOR_IS_NULL;
    if(label == NULL)
        return ARGUMENT_IS_NULL;
    if(vector->capacity < vector->size)
        return CAPACITY_LESS_THAN_SIZE;
    if(index >= vector->size)
        return INDEX_OUT_OF_RANGE;
    *label = vector->data[index];
    return OK;
}

int vector_pop(Vector* vector)
{
    if(vector == NULL)
        return VECTOR_IS_NULL;
    if(vector->capacity < vector->size)
        return CAPACITY_LESS_THAN_SIZE;
    if(vector->size == 0)
        return VECTOR_IS_EMPTY;
    --(vector->size);
    return OK;
}

int vector_push(Vector* vector, const Label* label_ref)
{
    if(vector == NULL)
        return VECTOR_IS_NULL;
    if(label_ref == NULL)
        return ARGUMENT_IS_NULL;
    if(vector->capacity < vector->size)
        return CAPACITY_LESS_THAN_SIZE;

    if(vector->size == vector->capacity) {
        if(vector->size == VECTOR_SIZE_MAX) {
            return VECTOR_OVERFLOW;
        }

        if(RESIZE_FACTOR * vector->capacity < vector->capacity) {
            if(vector_resize(vector, VECTOR_SIZE_MAX) == MEMORY_ALLOCATION_ERROR) {
                return MEMORY_ALLOCATION_ERROR;
            }
            else {
                (vector->data)[vector->size] = *label_ref;
                ++(vector->size);
                vector->capacity = VECTOR_SIZE_MAX;
            }
        }
        else {
            if(vector_resize(vector, RESIZE_FACTOR * vector->capacity) == MEMORY_ALLOCATION_ERROR)
                return MEMORY_ALLOCATION_ERROR;

            (vector->data)[vector->size] = *label_ref;
            ++(vector->size);
            vector->capacity *= RESIZE_FACTOR;
        }
    }
    // vector->size < vector->capacity
    else {
        (vector->data)[vector->size] = *label_ref;
        ++(vector->size);
    }
    return OK;
}

static int vector_resize(Vector* vector, size_t new_size)
{
    assert(vector != NULL);
    assert(vector->data != NULL);

    void* rez = (void*)realloc(vector->data, new_size * sizeof(Label));
    if(rez == NULL) {
        return MEMORY_ALLOCATION_ERROR;
    }
    vector->data = (Label*)rez;
    return OK;
}

