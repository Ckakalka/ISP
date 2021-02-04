/**
 * @file processor.c
 * Contains the function implementations of the processor as well as the definition of the processor structure
 */
#include "processor.h"
#include "../loader.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef _ASSERT_H_
#include <assert.h>
#endif



#define EPSILON 0.000001
#define NAME_MAX_SIZE 35

#define POINTER_POISON 0xF

#define is_zero(value) ((-EPSILON) < (value) && (value) < EPSILON)
#define TRUE 1
#define FALSE 0

#define STACK_PUSH TEMPLATE(stack_push, elem_cpu_t)
#define STACK_POP TEMPLATE(stack_pop, elem_cpu_t)
#define STACK_PEEK TEMPLATE(stack_peek, elem_cpu_t)
#define STACK_CREATE TEMPLATE(stack_create, elem_cpu_t)
#define STACK_DESTROY TEMPLATE(stack_destroy, elem_cpu_t)



struct processor {
    double ax;
    double bx;
    double cx;
    double dx;
    address_t next_command;
    TEMPLATE(stack, elem_cpu_t)* current_data;
    TEMPLATE(stack, TEMPLATE(stack, elem_cpu_t))* stacks;
    TEMPLATE(stack, address_t)* address_function_return;
};


static int processor_create(processor** cpu)
{
    assert(cpu != NULL);

    int rc = OK;

    *cpu = (processor*)calloc(1, sizeof(processor));
    if((*cpu) == NULL) {
        rc = MEMORY_ALLOCATION_ERROR;
        return rc;
    }

    (*cpu)->ax = 0;
    (*cpu)->bx = 0;
    (*cpu)->cx = 0;
    (*cpu)->dx = 0;
    (*cpu)->next_command = 0;

    rc = TEMPLATE(stack_create, address_t)(&((*cpu)->address_function_return), "stack_function_return_log.txt");
    if(rc != OK) {
        free(*cpu);
        return rc;
    }

    rc = TEMPLATE(stack_create, TEMPLATE(stack, elem_cpu_t))(&((*cpu)->stacks), "stack_of_stacks.txt");
    if(rc != OK) {
        TEMPLATE(stack_destroy, address_t)(&((*cpu)->address_function_return));
        free(*cpu);
        return rc;
    }

    rc = TEMPLATE(stack_create, elem_cpu_t)(&(*cpu)->current_data, "stack_log_0.txt");
    if(rc != OK) {
        TEMPLATE(stack_destroy, TEMPLATE(stack, elem_cpu_t))(&((*cpu)->stacks));
        TEMPLATE(stack_destroy, address_t)(&((*cpu)->address_function_return));
        free(*cpu);
        return rc;
    }

    rc = TEMPLATE(stack_ref_push, TEMPLATE(stack, elem_cpu_t))((*cpu)->stacks, (*cpu)->current_data);
    if(rc != OK) return rc;

    rc = TEMPLATE(stack_destroy_ref, elem_cpu_t)(&(*cpu)->current_data);
    if(rc != OK) return rc;

    rc = TEMPLATE(stack_ref_peek, TEMPLATE(stack, elem_cpu_t))((*cpu)->stacks, &(*cpu)->current_data);
    if(rc != OK) return rc;

    return rc;
}

static int processor_destroy(processor** cpu)
{
    assert(cpu != NULL);

    int rc = OK;

    rc = TEMPLATE(stack_destroy_data, elem_cpu_t)(&(*cpu)->current_data);
    if(rc != OK) return rc;

    rc = TEMPLATE(stack_destroy, TEMPLATE(stack, elem_cpu_t))(&((*cpu)->stacks));
    if(rc != OK) return rc;

    rc = TEMPLATE(stack_destroy, address_t)(&((*cpu)->address_function_return));
    if(rc != OK) return rc;

    free(*cpu);
    *cpu = (processor*)POINTER_POISON;

    return rc;
}

static int push(processor* cpu, elem_cpu_t value)
{
    assert(cpu != NULL);
    return STACK_PUSH(cpu->current_data, value);
}

static int push_reg(processor* cpu, char reg)
{
    assert(cpu != NULL);
    int rc = OK;
    switch (reg) {
        case AX:
            rc = STACK_PUSH(cpu->current_data, cpu->ax);
            break;
        case BX:
            rc = STACK_PUSH(cpu->current_data, cpu->bx);
            break;
        case CX:
            rc = STACK_PUSH(cpu->current_data, cpu->cx);
            break;
        case DX:
            rc = STACK_PUSH(cpu->current_data, cpu->dx);
            break;
        default:
            rc = UNRECOGNIZED_REGISTER;
    }
    return rc;
}

static int pop(processor* cpu)
{
    assert(cpu != NULL);
    return STACK_POP(cpu->current_data, NULL);
}

static int pop_reg(processor* cpu, char reg)
{
    assert(cpu != NULL);
    int rc = OK;
    switch (reg) {
        case AX:
            rc = STACK_POP(cpu->current_data, &(cpu->ax));
            break;
        case BX:
            rc = STACK_POP(cpu->current_data, &(cpu->bx));
            break;
        case CX:
            rc = STACK_POP(cpu->current_data, &(cpu->cx));
            break;
        case DX:
            rc = STACK_POP(cpu->current_data, &(cpu->dx));
            break;
        default:
            rc = UNRECOGNIZED_REGISTER;
    }
    return rc;
}

static int add(processor* cpu)
{
    assert(cpu != NULL);
    int rc = OK;
    double first_elem =  NAN;
    double second_elem = NAN;

    rc = STACK_POP(cpu->current_data, &(first_elem));
    if(rc != OK) return rc;

    rc = STACK_POP(cpu->current_data, &(second_elem));
    if(rc != OK) return rc;

    rc = STACK_PUSH(cpu->current_data, first_elem + second_elem);
    return rc;
}

static int sub(processor* cpu)
{
    assert(cpu != NULL);
    int rc = OK;
    double first_elem =  NAN;
    double second_elem = NAN;

    rc = STACK_POP(cpu->current_data, &(first_elem));
    if(rc != OK) return rc;

    rc = STACK_POP(cpu->current_data, &(second_elem));
    if(rc != OK) return rc;

    rc = STACK_PUSH(cpu->current_data, first_elem - second_elem);
    return rc;
}

static int mul(processor* cpu)
{
    assert(cpu != NULL);
    int rc = OK;
    double first_elem =  NAN;
    double second_elem = NAN;

    rc = STACK_POP(cpu->current_data, &(first_elem));
    if(rc != OK) return rc;

    rc = STACK_POP(cpu->current_data, &(second_elem));
    if(rc != OK) return rc;

    rc = STACK_PUSH(cpu->current_data, first_elem * second_elem);
    return rc;
}

static int div_cpu(processor* cpu)
{
    assert(cpu != NULL);
    int rc = OK;
    double first_elem =  NAN;
    double second_elem = NAN;

    rc = STACK_POP(cpu->current_data, &(first_elem));
    if(rc != OK) return rc;

    rc = STACK_POP(cpu->current_data, &(second_elem));
    if(rc != OK) return rc;

    if(is_zero(second_elem))
        return DIVISION_BY_ZERO;

    rc = STACK_PUSH(cpu->current_data, first_elem / second_elem);
    return rc;
}

static int sin_cpu(processor* cpu)
{
    assert(cpu != NULL);
    int rc = OK;
    double elem = NAN;

    rc = STACK_POP(cpu->current_data, &(elem));
    if(rc != OK) return rc;

    rc = STACK_PUSH(cpu->current_data, sin(elem));
    return rc;
}

static int cos_cpu(processor* cpu)
{
    assert(cpu != NULL);
    int rc = OK;
    double elem = NAN;

    rc = STACK_POP(cpu->current_data, &(elem));
    if(rc != OK) return rc;

    rc = STACK_PUSH(cpu->current_data, cos(elem));
    return rc;
}

static int sqrt_cpu(processor* cpu)
{
    assert(cpu != NULL);
    int rc = OK;
    double elem = NAN;

    rc = STACK_POP(cpu->current_data, &(elem));
    if(rc != OK) return rc;

    if(elem < -EPSILON)
        return ROOT_OF_NEGATIVE_VALUE;

    rc = STACK_PUSH(cpu->current_data, sqrt(elem));
    return rc;
}

static int jmp(processor* cpu, address_t offset)
{
    assert(cpu != NULL);
    cpu->next_command += offset;
    return JMP_WAS_MADE;
}

static int je(processor* cpu, address_t offset)
{
    assert(cpu != NULL);
    int rc = OK;
    double first_elem =  NAN;
    double second_elem = NAN;

    rc = STACK_POP(cpu->current_data, &(first_elem));
    if(rc != OK) return rc;

    rc = STACK_POP(cpu->current_data, &(second_elem));
    if(rc != OK) return rc;

    if(is_zero(first_elem - second_elem))
        return jmp(cpu, offset);
    return JMP_WAS_NOT_MADE;
}

static int jne(processor* cpu, address_t offset)
{
    assert(cpu != NULL);
    int rc = OK;
    double first_elem =  NAN;
    double second_elem = NAN;

    rc = STACK_POP(cpu->current_data, &(first_elem));
    if(rc != OK) return rc;

    rc = STACK_POP(cpu->current_data, &(second_elem));
    if(rc != OK) return rc;

    if(!is_zero(first_elem - second_elem))
        return jmp(cpu, offset);
    return JMP_WAS_NOT_MADE;
}

static int ja(processor* cpu, address_t offset)
{
    assert(cpu != NULL);
    int rc = OK;
    double first_elem =  NAN;
    double second_elem = NAN;

    rc = STACK_POP(cpu->current_data, &(first_elem));
    if(rc != OK) return rc;

    rc = STACK_POP(cpu->current_data, &(second_elem));
    if(rc != OK) return rc;

    if(first_elem - second_elem > EPSILON)
        return jmp(cpu, offset);
    return JMP_WAS_NOT_MADE;
}

static int jae(processor* cpu, address_t offset)
{
    assert(cpu != NULL);
    int rc = OK;
    double first_elem =  NAN;
    double second_elem = NAN;

    rc = STACK_POP(cpu->current_data, &(first_elem));
    if(rc != OK) return rc;

    rc = STACK_POP(cpu->current_data, &(second_elem));
    if(rc != OK) return rc;

    if(first_elem - second_elem > EPSILON || is_zero(first_elem - second_elem))
        return jmp(cpu, offset);
    return JMP_WAS_NOT_MADE;
}

static int jb(processor* cpu, address_t offset)
{
    assert(cpu != NULL);
    int rc = OK;
    double first_elem =  NAN;
    double second_elem = NAN;

    rc = STACK_POP(cpu->current_data, &(first_elem));
    if(rc != OK) return rc;

    rc = STACK_POP(cpu->current_data, &(second_elem));
    if(rc != OK) return rc;

    if(first_elem - second_elem < -EPSILON)
        return jmp(cpu, offset);
    return JMP_WAS_NOT_MADE;
}

static int jbe(processor* cpu, address_t offset)
{
    assert(cpu != NULL);
    int rc = OK;
    double first_elem =  NAN;
    double second_elem = NAN;

    rc = STACK_POP(cpu->current_data, &(first_elem));
    if(rc != OK) return rc;

    rc = STACK_POP(cpu->current_data, &(second_elem));
    if(rc != OK) return rc;

    if(first_elem - second_elem < -EPSILON || is_zero(first_elem - second_elem))
        return jmp(cpu, offset);
    return JMP_WAS_NOT_MADE;
}

static int call(processor* cpu, address_t offset, size_t number_param, size_t stack_counter)
{
    assert(cpu != NULL);
    int rc = OK;

    char name[NAME_MAX_SIZE];
    snprintf(name, NAME_MAX_SIZE, "stack_log_%u.txt", stack_counter);

    TEMPLATE(stack, elem_cpu_t)* function_data = NULL;

    rc = STACK_CREATE(&function_data, name);
    if(rc != OK) return rc;

    elem_cpu_t param;
    for(size_t i = 0; i < number_param; ++i) {
        rc = STACK_POP(cpu->current_data, &param);
        if(rc != OK) return rc;

        rc = STACK_PUSH(function_data, param);
        if(rc != OK) return rc;
    }

    rc = push_reg(cpu, AX);
    if(rc != OK) return rc;

    rc = push_reg(cpu, BX);
    if(rc != OK) return rc;

    rc = push_reg(cpu, CX);
    if(rc != OK) return rc;

    rc = push_reg(cpu, DX);
    if(rc != OK) return rc;

    rc = TEMPLATE(stack_ref_push, TEMPLATE(stack, elem_cpu_t))(cpu->stacks, function_data);
    if(rc != OK) return rc;

    rc = TEMPLATE(stack_destroy_ref, elem_cpu_t)(&function_data);
    if(rc != OK) return rc;

    rc = TEMPLATE(stack_ref_peek, TEMPLATE(stack, elem_cpu_t))(cpu->stacks, &(cpu->current_data));
    if(rc != OK) return rc;

    rc = jmp(cpu, offset);
    return rc;
}

static int ret(processor* cpu, size_t number_param)
{
    assert(cpu != NULL);
    int rc = OK;

    TEMPLATE(stack, elem_cpu_t)* function_data = cpu->current_data;

    rc = TEMPLATE(stack_pop, TEMPLATE(stack, elem_cpu_t))(cpu->stacks, NULL);
    if(rc != OK) return rc;

    rc = TEMPLATE(stack_ref_peek, TEMPLATE(stack, elem_cpu_t))(cpu->stacks, &(cpu->current_data));
    if(rc != OK) return rc;

    rc = pop_reg(cpu, DX);
    if(rc != OK) return rc;

    rc = pop_reg(cpu, CX);
    if(rc != OK) return rc;

    rc = pop_reg(cpu, BX);
    if(rc != OK) return rc;

    rc = pop_reg(cpu, AX);
    if(rc != OK) return rc;

    elem_cpu_t param;
    for(size_t i = 0; i < number_param; ++i) {
        rc = STACK_POP(function_data, &param);
        if (rc != OK) return rc;

        rc = STACK_PUSH(cpu->current_data, param);
        if (rc != OK) return rc;
    }

    rc = TEMPLATE(stack_destroy_data, elem_cpu_t)(&function_data);
    if(rc != OK) return rc;

    rc = TEMPLATE(stack_pop, address_t)(cpu->address_function_return, &(cpu->next_command));
    return rc;
}

static int out(processor* cpu)
{
    assert(cpu != NULL);
    int rc = OK;
    elem_cpu_t elem = 0;
    rc = STACK_PEEK(cpu->current_data, &elem);
    if(rc != OK) return rc;
    printf("%.3f\n", elem);
    return rc;
}

/**
 * Executes the program at the specified \b path
 * @param path File path
 * @return See return_codes.md
 */
int run(const char* path)
{
    if(path == NULL)
        return ARGUMENT_IS_NULL;

    int rc = OK;

    FileMapping file_mapping;
    rc = load_file(path, &file_mapping);
    if(rc != OK) return rc;

    processor* cpu = NULL;
    rc = processor_create(&cpu);
    if(rc != OK) {
        unmap_file(&file_mapping);
        return rc;
    }

    rc = execute(cpu, &file_mapping);

    unmap_file(&file_mapping);
    processor_destroy(&cpu);
    return rc;
}

static int execute(processor* cpu, FileMapping* file_mapping)
{
    assert(cpu != NULL);
    assert(file_mapping != NULL);
    int rc = OK;
    int is_stop = FALSE;

    size_t stack_counter = 1;

    cpu->next_command = 0;

    while(cpu->next_command <= file_mapping->size && !is_stop) {
        rc = OK;
        switch (file_mapping->program[cpu->next_command]) {
            case PUSH:
                if(cpu->next_command + COMMAND_SIZE + sizeof(elem_cpu_t) > file_mapping->size)
                    return PROGRAM_BOUNDARY_ERROR;
                rc = push(cpu, *(elem_cpu_t*)(file_mapping->program + cpu->next_command + COMMAND_SIZE));
                cpu->next_command += COMMAND_SIZE + sizeof(elem_cpu_t);
                break;
            case PUSH_REG:
                if(cpu->next_command + COMMAND_SIZE + sizeof(char) > file_mapping->size)
                    return PROGRAM_BOUNDARY_ERROR;
                rc = push_reg(cpu, *(char*)(file_mapping->program + cpu->next_command + COMMAND_SIZE));
                cpu->next_command += COMMAND_SIZE + sizeof(char);
                break;
            case POP:
                rc = pop(cpu);
                cpu->next_command += COMMAND_SIZE;
                break;
            case POP_REG:
                if(cpu->next_command + COMMAND_SIZE + sizeof(char) > file_mapping->size)
                    return PROGRAM_BOUNDARY_ERROR;
                rc = pop_reg(cpu, *(char*)(file_mapping->program + cpu->next_command + COMMAND_SIZE));
                cpu->next_command += COMMAND_SIZE + sizeof(char);
                break;
            case ADD:
                rc = add(cpu);
                cpu->next_command += COMMAND_SIZE;
                break;
            case SUB:
                rc = sub(cpu);
                cpu->next_command += COMMAND_SIZE;
                break;
            case MUL:
                rc = mul(cpu);
                cpu->next_command += COMMAND_SIZE;
                break;
            case DIV:
                rc = div_cpu(cpu);
                cpu->next_command += COMMAND_SIZE;
                break;
            case SIN:
                rc = sin_cpu(cpu);
                cpu->next_command += COMMAND_SIZE;
                break;
            case COS:
                rc = cos_cpu(cpu);
                cpu->next_command += COMMAND_SIZE;
                break;
            case SQRT:
                rc = sqrt_cpu(cpu);
                cpu->next_command += COMMAND_SIZE;
                break;
            case JMP:
                if(cpu->next_command + COMMAND_SIZE + sizeof(address_t) > file_mapping->size)
                    return PROGRAM_BOUNDARY_ERROR;
                rc = jmp(cpu, *(address_t*)(file_mapping->program + cpu->next_command + COMMAND_SIZE));
                break;
            case JE:
                if(cpu->next_command + COMMAND_SIZE + sizeof(address_t) > file_mapping->size)
                    return PROGRAM_BOUNDARY_ERROR;
                rc = je(cpu, *(address_t*)(file_mapping->program + cpu->next_command + COMMAND_SIZE));
                if(rc != JMP_WAS_MADE)
                    cpu->next_command += COMMAND_SIZE + sizeof(address_t);
                break;
            case JNE:
                if(cpu->next_command + COMMAND_SIZE + sizeof(address_t) > file_mapping->size)
                    return PROGRAM_BOUNDARY_ERROR;
                rc = jne(cpu, *(address_t*)(file_mapping->program + cpu->next_command + COMMAND_SIZE));
                if(rc != JMP_WAS_MADE)
                    cpu->next_command += COMMAND_SIZE + sizeof(address_t);
                break;
            case JA:
                if(cpu->next_command + COMMAND_SIZE + sizeof(address_t) > file_mapping->size)
                    return PROGRAM_BOUNDARY_ERROR;
                rc = ja(cpu, *((const address_t*)(file_mapping->program + cpu->next_command + COMMAND_SIZE)));
                if(rc != JMP_WAS_MADE)
                    cpu->next_command += COMMAND_SIZE + sizeof(address_t);
                break;
            case JAE:
                if(cpu->next_command + COMMAND_SIZE + sizeof(address_t) > file_mapping->size)
                    return PROGRAM_BOUNDARY_ERROR;
                rc = jae(cpu, *(address_t*)(file_mapping->program + cpu->next_command + COMMAND_SIZE));
                if(rc != JMP_WAS_MADE)
                    cpu->next_command += COMMAND_SIZE + sizeof(address_t);
                break;
            case JB:
                if(cpu->next_command + COMMAND_SIZE + sizeof(address_t) > file_mapping->size)
                    return PROGRAM_BOUNDARY_ERROR;
                rc = jb(cpu, *(address_t*)(file_mapping->program + cpu->next_command + COMMAND_SIZE));
                if(rc != JMP_WAS_MADE)
                    cpu->next_command += COMMAND_SIZE + sizeof(address_t);
                break;
            case JBE:
                if(cpu->next_command + COMMAND_SIZE + sizeof(address_t) > file_mapping->size)
                    return PROGRAM_BOUNDARY_ERROR;
                rc = jbe(cpu, *(address_t*)(file_mapping->program + cpu->next_command + COMMAND_SIZE));
                if(rc != JMP_WAS_MADE)
                    cpu->next_command += COMMAND_SIZE + sizeof(address_t);
                break;
            case CALL:
                if(cpu->next_command + COMMAND_SIZE + sizeof(address_t) + sizeof(size_t) > file_mapping->size)
                    return PROGRAM_BOUNDARY_ERROR;
                rc = TEMPLATE(stack_push, address_t)(cpu->address_function_return,
                                                     cpu->next_command + COMMAND_SIZE + sizeof(address_t) + sizeof(size_t));
                if(rc != OK) return rc;
                rc = call(cpu, *(address_t*)(file_mapping->program + cpu->next_command + COMMAND_SIZE),
                          *(size_t*)(file_mapping->program + cpu->next_command + COMMAND_SIZE + sizeof(address_t)), stack_counter);
                ++stack_counter;
                break;
            case RET:
                if(cpu->next_command + COMMAND_SIZE + sizeof(size_t) > file_mapping->size)
                    return PROGRAM_BOUNDARY_ERROR;
                rc = ret(cpu, *(size_t*)(file_mapping->program + cpu->next_command + COMMAND_SIZE));
                break;
            case STOP:
                cpu->next_command += COMMAND_SIZE;
                is_stop = TRUE;
                rc = OK;
                break;
            case OUT:
                cpu->next_command += COMMAND_SIZE;
                rc = out(cpu);
                break;
            default:
                rc = UNRECOGNIZED_COMMAND;
                break;
        }
        if(rc != OK && rc != JMP_WAS_MADE && rc != JMP_WAS_NOT_MADE) {
            return rc;
        }
    }
    if(!is_stop) {
        return INCORRECT_TERMINATION;
    }
    return rc;
}


















