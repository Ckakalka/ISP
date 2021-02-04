/**
 * @file stack_types.h
 * Defines the supported stack types and connects the function headers of these types
 */
#ifndef STACK_POSSIBLE_TYPES_H
#define STACK_POSSIBLE_TYPES_H
#include <stdint.h>
#include "templates.h"
#include "../processor/cpu_types.h"

#undef T
#define T elem_cpu_t
#include "stack_templates.h"

#undef T
#define T address_t
#include "stack_templates.h"

#undef T
#define T TEMPLATE(stack, elem_cpu_t)
#include "stack_templates.h"

#endif //STACK_POSSIBLE_TYPES_H
