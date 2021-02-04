/**
 * @file processor.h
 * Contains the processor function headers and the processor structure declaration
 */
#ifndef CPU_PROCESSOR_H
#define CPU_PROCESSOR_H

#include "../stack/stack_types.h"
#include "../structs_and_enums.h"



typedef struct processor processor;


int run(const char* path);

static int processor_create(processor** cpu);
static int processor_destroy(processor** cpu);
static int push(processor* cpu, elem_cpu_t value);
static int push_reg(processor* cpu, char reg);
static int pop(processor* cpu);
static int pop_reg(processor* cpu, char reg);
static int add(processor* cpu);
static int sub(processor* cpu);
static int mul(processor* cpu);
static int div_cpu(processor* cpu);
static int sin_cpu(processor* cpu);
static int cos_cpu(processor* cpu);
static int sqrt_cpu(processor* cpu);
static int out(processor* cpu);


static int jmp(processor* cpu, address_t offset);
static int je(processor* cpu, address_t offset);
static int jne(processor* cpu, address_t offset);
static int ja(processor* cpu, address_t offset);
static int jae(processor* cpu, address_t offset);
static int jb(processor* cpu, address_t offset);
static int jbe(processor* cpu, address_t offset);

static int call(processor* cpu, address_t offset, size_t number_param, size_t stack_counter);
static int ret(processor* cpu, size_t number_param);

static int execute(processor* cpu, FileMapping* file_mapping);

#endif //CPU_PROCESSOR_H
