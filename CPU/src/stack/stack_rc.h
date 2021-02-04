/**
 * @file stack_rc.c
 * Contains stack return codes
 */
#ifndef CPU_STACK_RC_H
#define CPU_STACK_RC_H

typedef enum stack_rc {
    OK = 0,
    STACK_IS_NULL,
    ARGUMENT_IS_NULL,
    MEMORY_ALLOCATION_ERROR,
    STACK_ALREADY_DELETED,
    LEFT_CANARY_CHANGED,
    RIGHT_CANARY_CHANGED,
    STACK_OVERFLOW,
    STACK_IS_EMPTY,
    CAPACITY_LESS_THAN_SIZE = 9
} stack_rc;

#endif //CPU_STACK_RC_H
