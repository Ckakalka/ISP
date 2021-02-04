/**
 * @file stack_templates.c
 * Contains the function implementations of the stack
 */

#ifdef T

#include <stddef.h>
#include <malloc.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#ifndef _ASSERT_H_
#include <assert.h>
#endif

#include "templates.h"
#include "stack_rc.h"

// sizeof(canary) == sizeof(T)
#define CANARY_SIZE 2

#define CANARY_VALUE 73
#define INITIAL_CAPACITY 10
#define POINTER_POISON 0xF
#define DEFAULT_VALUE 0
#define RESIZE_FACTOR 2
#define STACK_SIZE_MAX (SIZE_MAX - 2 * CANARY_SIZE)



#define VERIFY_STACK(stack_ptr)                                                                             \
    if((stack_ptr) == NULL) {                                                                               \
        rc = STACK_IS_NULL;                                                                                 \
        goto EXIT_BLOCK;                                                                                    \
    }                                                                                                       \
    if((stack_ptr) == (TEMPLATE(stack, T)*)POINTER_POISON) {                                                \
        rc = STACK_ALREADY_DELETED;                                                                         \
        goto EXIT_BLOCK;                                                                                    \
    }                                                                                                       \
    if((stack_ptr)->capacity < (stack_ptr)->size) {                                                         \
        rc = CAPACITY_LESS_THAN_SIZE;                                                                       \
        goto EXIT_BLOCK;                                                                                    \
    }                                                                                                       \
    for(size_t i = 0; i < CANARY_SIZE * sizeof(T); ++i)                                                     \
        if(*(((char *)(stack_ptr)->data) + i) != CANARY_VALUE) {                                            \
            rc = LEFT_CANARY_CHANGED;                                                                       \
            goto EXIT_BLOCK;                                                                                \
        }                                                                                                   \
    for(size_t i = (CANARY_SIZE + (stack_ptr)->capacity) * sizeof(T);                                       \
                i < (2 * CANARY_SIZE + (stack_ptr)->capacity) * sizeof(T); ++i)                             \
        if(*(((char *)(stack_ptr)->data) + i) != CANARY_VALUE) {                                            \
            rc = RIGHT_CANARY_CHANGED;                                                                      \
            goto EXIT_BLOCK;                                                                                \
        }

/**
 * Creates a stack with the specified name. The name is used for the name of the log file
 * @param stack The pointer that will be used to write the address of the created stack
 * @param name The name of the stack
 * @return See return_codes.md
 */
int TEMPLATE(stack_create, T)(TEMPLATE(stack, T)** stack, char* name)
{
    int rc = OK;

    if(stack == NULL) {
        rc = ARGUMENT_IS_NULL;
        return rc;
    }

    if(name == NULL) {
        rc = ARGUMENT_IS_NULL;
        return rc;
    }

    *stack = (TEMPLATE(stack, T)*)calloc(1, sizeof(TEMPLATE(stack, T)));
    if((*stack) == NULL) {
        rc = MEMORY_ALLOCATION_ERROR;
        return rc;
    }

    size_t data_number = INITIAL_CAPACITY + 2 * CANARY_SIZE;
    (*stack)->data = (T*)calloc(data_number, sizeof(T));
    if((*stack) == NULL) {
        free(*stack);
        rc = MEMORY_ALLOCATION_ERROR;
        return rc;
    }

    (*stack)->size = 0;
    (*stack)->capacity = INITIAL_CAPACITY;
    (*stack)->name = (char*)calloc(strlen(name) + 1, sizeof(char));
    strcpy((*stack)->name, name);

    memset((*stack)->data, CANARY_VALUE, CANARY_SIZE * sizeof(T));
    memset((*stack)->data + CANARY_SIZE + INITIAL_CAPACITY, CANARY_VALUE, CANARY_SIZE * sizeof(T));

    // TODO Implement hash protector

    return rc;
}

/**
 * Deletes the stack and clears the memory allocated for it and the memory allocated for the name and data
 * @param stack The address of the stack pointer
 * @return See return_codes.md
 */
int TEMPLATE(stack_destroy, T)(TEMPLATE(stack, T)** stack)
{
    int rc = OK;

    if(stack == NULL) {
        rc= ARGUMENT_IS_NULL;
        goto EXIT_BLOCK;
    }
    VERIFY_STACK(*stack)

    free((*stack)->name);
    memset((*stack)->data, DEFAULT_VALUE, (2 * CANARY_SIZE + (*stack)->capacity) * sizeof(T));
    free((*stack)->data);
    (*stack)->data = (T*) POINTER_POISON;

    memset(*stack, DEFAULT_VALUE, sizeof(TEMPLATE(stack, T)));
    free(*stack);
    *stack = (TEMPLATE(stack, T)*)POINTER_POISON;

    EXIT_BLOCK:
    if(rc != OK)
        TEMPLATE(dump, T)("stack_destroy", rc, *stack);
    return rc;
}

/**
 * Deletes the stack reference and clears the memory allocated for it.
 * Attention: Does not free up memory for the name and data of the stack!
 * @param stack The address of the stack pointer
 * @return See return_codes.md
 */
int TEMPLATE(stack_destroy_ref, T)(TEMPLATE(stack, T)** stack)
{
    int rc = OK;

    if(stack == NULL) {
        rc= ARGUMENT_IS_NULL;
        goto EXIT_BLOCK;
    }
    VERIFY_STACK(*stack)
    memset(*stack, DEFAULT_VALUE, sizeof(TEMPLATE(stack, T)));
    free(*stack);
    *stack = (TEMPLATE(stack, T)*)POINTER_POISON;

    EXIT_BLOCK:
    if(rc != OK)
        TEMPLATE(dump, T)("stack_destroy_ref", rc, *stack);
    return rc;
}

/**
 * Deletes the stack data and clears the memory allocated for it.
 * Attention: Does not free up memory for stack reference
 * @param stack The address of the stack pointer
 * @return See return_codes.md
 */
int TEMPLATE(stack_destroy_data, T)(TEMPLATE(stack, T)** stack)
{
    int rc = OK;

    if(stack == NULL) {
        rc= ARGUMENT_IS_NULL;
        goto EXIT_BLOCK;
    }
    VERIFY_STACK(*stack)

    free((*stack)->name);
    memset((*stack)->data, DEFAULT_VALUE, (2 * CANARY_SIZE + (*stack)->capacity) * sizeof(T));
    free((*stack)->data);
    (*stack)->data = (T*) POINTER_POISON;

    EXIT_BLOCK:
    if(rc != OK)
        TEMPLATE(dump, T)("stack_destroy_data", rc, *stack);
    return rc;
}

/**
 * Writes the size of \b stack to the address \b size
 * @param stack The stack pointer
 * @param size The address where the size will be record
 * @return See return_codes.md
 */
int TEMPLATE(stack_get_size, T)(TEMPLATE(stack, T)* stack, size_t* size)
{
    int rc = OK;

    if(size == NULL) {
        rc = ARGUMENT_IS_NULL;
        goto EXIT_BLOCK;
    }
    VERIFY_STACK(stack)

    *size = stack->size;

    EXIT_BLOCK:
    TEMPLATE(dump, T)("stack_get_size", rc, stack);
    return rc;
}

/**
 * Writes the capacity of \b stack to the address \b size
 * @param stack The stack pointer
 * @param size The address where the capacity will be record
 * @return See return_codes.md
 */
int TEMPLATE(stack_get_capacity, T)(TEMPLATE(stack, T)* stack, size_t* capacity)
{
    int rc = OK;

    if(capacity == NULL) {
        rc = ARGUMENT_IS_NULL;
        goto EXIT_BLOCK;
    }
    VERIFY_STACK(stack)

    *capacity = stack->capacity;

    EXIT_BLOCK:
    TEMPLATE(dump, T)("stack_get_capacity", rc, stack);
    return rc;
}

/**
 * Puts the \b elem at the end of the \b stack
 * @param stack The stack pointer
 * @param elem Element to push
 * @return See return_codes.md
 */
int TEMPLATE(stack_push, T)(TEMPLATE(stack, T)* stack, T elem)
{
    int rc = OK;
    VERIFY_STACK(stack)

    if(stack->size == stack->capacity) {
        if(stack->size == STACK_SIZE_MAX) {
            rc = STACK_OVERFLOW;
            goto EXIT_BLOCK;
        }

        if(RESIZE_FACTOR * stack->capacity < stack->capacity) {
            if(TEMPLATE(stack_resize, T)(stack, STACK_SIZE_MAX + 2 * CANARY_SIZE) == MEMORY_ALLOCATION_ERROR) {
                rc = MEMORY_ALLOCATION_ERROR;
                goto EXIT_BLOCK;
            }
            else {
                (stack->data)[stack->size + CANARY_SIZE] = elem;
                ++(stack->size);
                stack->capacity = STACK_SIZE_MAX;
            }
        }
        else {
            if(RESIZE_FACTOR * stack->capacity >= STACK_SIZE_MAX) {
                rc = TEMPLATE(stack_resize, T)(stack, STACK_SIZE_MAX + 2 * CANARY_SIZE);
                if(rc == MEMORY_ALLOCATION_ERROR)
                    goto EXIT_BLOCK;
                stack->capacity = STACK_SIZE_MAX;
            }
            else {
                rc = TEMPLATE(stack_resize, T)(stack, RESIZE_FACTOR * stack->capacity + 2 * CANARY_SIZE);
                if(rc == MEMORY_ALLOCATION_ERROR)
                    goto EXIT_BLOCK;
                stack->capacity *= RESIZE_FACTOR;
            }
                (stack->data)[stack->size + CANARY_SIZE] = elem;
                ++(stack->size);
        }
    }
    // stack->size < stack->capacity
    else {
        (stack->data)[stack->size + CANARY_SIZE] = elem;
        ++(stack->size);
    }

    EXIT_BLOCK:
    TEMPLATE(dump, T)("stack_push", rc, stack);
    return rc;
}

/**
 * Puts the elem located at \b elem_ref at the end of the stack
 * @param stack The stack pointer
 * @param elem Address of element to push
 * @return See return_codes.md
 */
int TEMPLATE(stack_ref_push, T)(TEMPLATE(stack, T)* stack, T* elem_ref)
{
    int rc = OK;

    if(elem_ref == NULL) {
        rc= ARGUMENT_IS_NULL;
        goto EXIT_BLOCK;
    }
    VERIFY_STACK(stack)

    if(stack->size == stack->capacity) {
        if(stack->size == STACK_SIZE_MAX) {
            rc = STACK_OVERFLOW;
            goto EXIT_BLOCK;
        }

        if(RESIZE_FACTOR * stack->capacity < stack->capacity) {
            if(TEMPLATE(stack_resize, T)(stack, STACK_SIZE_MAX + 2 * CANARY_SIZE) == MEMORY_ALLOCATION_ERROR) {
                rc = MEMORY_ALLOCATION_ERROR;
                goto EXIT_BLOCK;
            }
            else {
                (stack->data)[stack->size + CANARY_SIZE] = *elem_ref;
                ++(stack->size);
                stack->capacity = STACK_SIZE_MAX;
            }
        }
        else {
            if(RESIZE_FACTOR * stack->capacity >= STACK_SIZE_MAX) {
                rc = TEMPLATE(stack_resize, T)(stack, STACK_SIZE_MAX + 2 * CANARY_SIZE);
                if(rc == MEMORY_ALLOCATION_ERROR)
                    goto EXIT_BLOCK;
                stack->capacity = STACK_SIZE_MAX;
            }
            else {
                rc = TEMPLATE(stack_resize, T)(stack, RESIZE_FACTOR * stack->capacity + 2 * CANARY_SIZE);
                if(rc == MEMORY_ALLOCATION_ERROR)
                    goto EXIT_BLOCK;
                stack->capacity *= RESIZE_FACTOR;
            }
            (stack->data)[stack->size + CANARY_SIZE] = *elem_ref;
            ++(stack->size);
        }
    }
        // stack->size < stack->capacity
    else {
        (stack->data)[stack->size + CANARY_SIZE] = *elem_ref;
        ++(stack->size);
    }

    EXIT_BLOCK:
    TEMPLATE(dump, T)("stack_ref_push", rc, stack);
    return rc;
}

/**
 * Pops the last element from the \b stack and puts it at the address \b elem
 * If the elem is NULL then just pops the last elem from the stack
 * @param stack The stack pointer
 * @param elem The address where the element will be record
 * @return See return_codes.md
 */
int TEMPLATE(stack_pop, T)(TEMPLATE(stack, T)* stack, T* elem)
{
    int rc = OK;

    VERIFY_STACK(stack)

    if(stack->size == 0) {
        rc = STACK_IS_EMPTY;
        goto EXIT_BLOCK;
    }

    if(elem != NULL)
        *elem = (stack->data)[stack->size + CANARY_SIZE - 1];

    --(stack->size);


    EXIT_BLOCK:
    TEMPLATE(dump, T)("stack_pop", rc, stack);
    return rc;
}

/**
 * Pops the last element from the \b stack and puts it address at the address \b elem
 * @param stack The stack pointer
 * @param elem The address where the address of element will be record
 * @return See return_codes.md
 */
int TEMPLATE(stack_ref_pop, T)(TEMPLATE(stack, T)* stack, T** elem)
{
    int rc = OK;

    if(elem == NULL) {
        rc = ARGUMENT_IS_NULL;
        goto EXIT_BLOCK;
    }
    VERIFY_STACK(stack)

    if(stack->size == 0) {
        rc = STACK_IS_EMPTY;
        goto EXIT_BLOCK;
    }

    *elem = &((stack->data)[--(stack->size) + CANARY_SIZE]);

    EXIT_BLOCK:
    TEMPLATE(dump, T)("stack_ref_pop", rc, stack);
    return rc;
}

/**
 * Puts a copy of the last element on the \b stack at address \b elem
 * @param stack The stack pointer
 * @param elem The address where the element will be record
 * @return See return_codes.md
 */
int TEMPLATE(stack_peek, T)(TEMPLATE(stack, T)* stack, T* elem)
{
    int rc = OK;

    if(elem == NULL) {
        rc = ARGUMENT_IS_NULL;
        goto EXIT_BLOCK;
    }

    VERIFY_STACK(stack)

    if(stack->size == 0) {
        rc = STACK_IS_EMPTY;
        goto EXIT_BLOCK;
    }

    *elem = (stack->data)[stack->size + CANARY_SIZE - 1];

    EXIT_BLOCK:
    TEMPLATE(dump, T)("stack_peek", rc, stack);
    return rc;
}

/**
 * Puts a address of the last element on the \b stack at address \b elem
 * @param stack The stack pointer
 * @param elem The address where the address of element will be record
 * @return See return_codes.md
 */
int TEMPLATE(stack_ref_peek, T)(TEMPLATE(stack, T)* stack, T** elem)
{
    int rc = OK;

    if(elem == NULL) {
        rc = ARGUMENT_IS_NULL;
        goto EXIT_BLOCK;
    }

    VERIFY_STACK(stack)

    if(stack->size == 0) {
        rc = STACK_IS_EMPTY;
        goto EXIT_BLOCK;
    }

    *elem = &((stack->data)[stack->size + CANARY_SIZE - 1]);

    EXIT_BLOCK:
    TEMPLATE(dump, T)("stack_ref_peek", rc, stack);
    return rc;
}

/**
 * Clears the stack. Does not delete data
 * @param stack The stack pointer
 * @return See return_codes.md
 */
int TEMPLATE(stack_clear, T)(TEMPLATE(stack, T)* stack)
{
    int rc = OK;
    VERIFY_STACK(stack)

    stack->size = 0;

    EXIT_BLOCK:
    TEMPLATE(dump, T)("stack_clear", rc, stack);
    return rc;
}

static int TEMPLATE(stack_resize, T)(TEMPLATE(stack, T)* stack, size_t new_size)
{
    assert(stack != NULL);
    assert(stack->name != NULL);
    assert(stack->data != NULL);

    int rc = OK;

    void* rez = (void*) realloc(stack->data, new_size * sizeof(T));
    if(rez == NULL) {
        rc = MEMORY_ALLOCATION_ERROR;
        goto EXIT_BLOCK;
    }

    stack->data = (T*) rez;
    memset(stack->data + new_size - CANARY_SIZE, CANARY_VALUE, CANARY_SIZE * sizeof(T));

    EXIT_BLOCK:
    return rc;
}
static void TEMPLATE(dump, T)(char* function_name, int rc, TEMPLATE(stack, T)* stack)
{
    assert(stack != NULL);
    assert(function_name != NULL);

    FILE* log = fopen(stack->name, "a");
    if(log == NULL)
        return;
    switch(rc) {
        case STACK_IS_NULL:
        case STACK_ALREADY_DELETED:
            fprintf(log, "%s: rc = %d stack* = %p\n\n", function_name, rc, stack);
            break;
        case CAPACITY_LESS_THAN_SIZE:
            fprintf(log, "%s: rc = %d ; stack* = %p ; stack.size = %u ; stack.capacity = %u ; stack.program = %p\n\n",
                    function_name, rc, stack, stack->size, stack->capacity, stack->data);
            break;
        case OK:
        case MEMORY_ALLOCATION_ERROR:
        case STACK_IS_EMPTY:
        case LEFT_CANARY_CHANGED:
        case RIGHT_CANARY_CHANGED:
        case STACK_OVERFLOW:
            fprintf(log, "%s: rc = %d ; stack* = %p ; stack.size = %u ; stack.capacity = %u ; stack.program = %p\n",
                    function_name, rc, stack, stack->size, stack->capacity, stack->data);
            for (size_t i = CANARY_SIZE; i < stack->capacity + CANARY_SIZE; ++i) {
                fprintf(log, "stack.program[%d]: ", i);
                TEMPLATE(print_log, T)(log, &((stack->data)[i]));
            }
            fprintf(log, "\n");
            break;
        default:
            fprintf(log, "%s: rc = %d stack* = %p\n\n", function_name, rc, stack);
            break;
    }
    if(fclose(log) != 0)
        printf("Failed to close log\n");
}

#endif
