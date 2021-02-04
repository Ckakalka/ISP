/**
 * @file assembler.c
 * Contains function implementations for file assembly
 */
#include "assembler.h"
#include "../processor/cpu_types.h"
#include "../vector_label.h"
#include "../loader.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <stdint.h>

#ifndef _ASSERT_H_
#include <assert.h>
#endif


#define BUFFER_SIZE 512

#define BUFFER_WRITE_IF_NEEDED(size_next_elem) \
    if(offset_output % BUFFER_SIZE + (size_next_elem) >= BUFFER_SIZE) { \
        number_elem = fwrite(buffer_output, sizeof(char), offset_output % BUFFER_SIZE, assemble_file); \
        if(number_elem != offset_output % BUFFER_SIZE) { \
            rc = ERROR_FILE_WRITE; \
            goto EXIT_BLOCK; \
        } \
    }

/**
 * Assembles the file \b input into \b output
 * @param input Path to the file for assembly
 * @param output Path to the assembled file
 * @param line_number Value to write the number of the last successfully assembled line
 * @return See return_codes.md
 */
int assemble(const char* input, const char* output, size_t* line_number)
{
    if(input == NULL || output == NULL || line_number == NULL)
        return ARGUMENT_IS_NULL;

    int rc = OK;
    FileMapping file_mapping;

    rc = load_file(input, &file_mapping);
    if(rc != OK) return rc;

    FILE* assemble_file = NULL;
    assemble_file = fopen(output, "wb");
    if(assemble_file == NULL) {
        unmap_file(&file_mapping);
        return ERROR_FILE_OPEN;
    }

    Vector* labels_address = NULL;
    Vector* labels_jmp = NULL;

    rc = vector_create(&labels_address);
    if(rc != OK) {
        unmap_file(&file_mapping);
        fclose(assemble_file);
        return rc;
    }
    rc = vector_create(&labels_jmp);
    if(rc != OK) {
        vector_destroy(&labels_address);
        unmap_file(&file_mapping);
        fclose(assemble_file);
        return rc;
    }

    Label tmp_label;
    size_t offset_input = 0;
    size_t word_size = 0;

    char buffer_output[BUFFER_SIZE];
    size_t offset_output = 0;

    char* command_mnemonics[NUMBER_COMMAND_MNEMONICS] = {"PUSH", "POP", "ADD", "SUB", "MUL",
                                                         "DIV", "SIN", "COS", "SQRT", "JMP", "JE", "JNE", "JA",
                                                         "JAE", "JB", "JBE", "CALL", "RET", "STOP", "OUT"};

    char* register_mnemonics[NUMBER_REGISTERS] = {"ax", "bx", "cx", "dx"};

    char command_mnemonic = NUMBER_COMMAND_MNEMONICS;
    char register_code = NUMBER_REGISTERS;
    char command_code = NUMBER_COMMANDS;
    int is_read_next_word = FALSE;
    size_t number_elem = 0;

    read_word(&file_mapping, &offset_input, &word_size, line_number);
    while(word_size != 0) {
        command_mnemonic = NUMBER_COMMAND_MNEMONICS;
        register_code = NUMBER_REGISTERS;
        command_code = NUMBER_COMMANDS;
        is_read_next_word = TRUE;
        number_elem = 0;
        for(char i = 0; i < NUMBER_COMMAND_MNEMONICS; ++i) {
            if(str_is_equal_i(file_mapping.program + offset_input, command_mnemonics[i]) == TRUE) {
                command_mnemonic = i;
                break;
            }
        }

        switch (command_mnemonic) {
            case PUSH_M:
                read_word(&file_mapping, &offset_input, &word_size, line_number);
                if(word_size == 0) {
                    rc = EXPECTED_ARGUMENT;
                    goto EXIT_BLOCK;
                }
                char* end_ptr = NULL;
                elem_cpu_t value = strtod(file_mapping.program + offset_input, &end_ptr);
                if(value == HUGE_VAL || value == (-HUGE_VAL)) {
                    rc = INVALID_ARGUMENT;
                    goto EXIT_BLOCK;
                }
                // if it is push register
                if(value == 0 && end_ptr == file_mapping.program + offset_input) {
                    register_code = NUMBER_REGISTERS;
                    for(char i = 0; i < NUMBER_REGISTERS; ++i) {
                        if (str_is_equal_i(file_mapping.program + offset_input, register_mnemonics[i]) == TRUE) {
                            register_code = i;
                            break;
                        }
                    }
                    if(register_code == AX || register_code == BX
                       || register_code == CX || register_code == DX) {
                        BUFFER_WRITE_IF_NEEDED(COMMAND_SIZE)
                        buffer_output[offset_output % BUFFER_SIZE] = PUSH_REG;
                        offset_output += COMMAND_SIZE;

                        BUFFER_WRITE_IF_NEEDED(REGISTER_SIZE)
                        buffer_output[offset_output % BUFFER_SIZE] = register_code;
                        offset_output += REGISTER_SIZE;
                    }
                    else {
                        rc = UNRECOGNIZED_REGISTER;
                        goto EXIT_BLOCK;
                    }
                }
                // it is a push value
                else {
                    BUFFER_WRITE_IF_NEEDED(COMMAND_SIZE)
                    buffer_output[offset_output % BUFFER_SIZE] = PUSH;
                    offset_output += COMMAND_SIZE;

                    char* ptr_value = (char*)(&value);
                    BUFFER_WRITE_IF_NEEDED(sizeof(elem_cpu_t))
                    for(size_t i = 0; i < sizeof(elem_cpu_t); ++i) {
                        buffer_output[offset_output % BUFFER_SIZE] = ptr_value[i];
                        ++offset_output;
                    }
                }
                break;
            case POP_M:
                read_word(&file_mapping, &offset_input, &word_size, line_number);
                register_code = NUMBER_REGISTERS;
                if(word_size != 0) {
                    for(char i = 0; i < NUMBER_REGISTERS; ++i) {
                        if (str_is_equal_i(file_mapping.program + offset_input, register_mnemonics[i]) == TRUE) {
                            register_code = i;
                            break;
                        }
                    }
                }
                // if pop_reg
                if(register_code == AX || register_code == BX
                   || register_code == CX || register_code == DX) {
                    BUFFER_WRITE_IF_NEEDED(COMMAND_SIZE)
                    buffer_output[offset_output % BUFFER_SIZE] = POP_REG;
                    offset_output += COMMAND_SIZE;

                    BUFFER_WRITE_IF_NEEDED(REGISTER_SIZE)
                    buffer_output[offset_output % BUFFER_SIZE] = register_code;
                    offset_output += REGISTER_SIZE;
                }
                // pop
                else {
                    BUFFER_WRITE_IF_NEEDED(COMMAND_SIZE)
                    buffer_output[offset_output % BUFFER_SIZE] = POP;
                    offset_output += COMMAND_SIZE;
                    is_read_next_word = FALSE;
                }
                break;
            case ADD_M:
            case SUB_M:
            case MUL_M:
            case DIV_M:
            case SIN_M:
            case COS_M:
            case SQRT_M:
                if(command_mnemonic == ADD_M) command_code = ADD;
                else if(command_mnemonic == SUB_M) command_code = SUB;
                else if(command_mnemonic == MUL_M) command_code = MUL;
                else if(command_mnemonic == DIV_M) command_code = DIV;
                else if(command_mnemonic == SIN_M) command_code = SIN;
                else if(command_mnemonic == COS_M) command_code = COS;
                else command_code = SQRT;

                BUFFER_WRITE_IF_NEEDED(COMMAND_SIZE)
                buffer_output[offset_output % BUFFER_SIZE] = command_code;
                offset_output += COMMAND_SIZE;
                break;
            case JMP_M:
            case JE_M:
            case JNE_M:
            case JA_M:
            case JAE_M:
            case JB_M:
            case JBE_M:
                if(command_mnemonic == JMP_M) command_code = JMP;
                else if(command_mnemonic == JE_M) command_code = JE;
                else if(command_mnemonic == JNE_M) command_code = JNE;
                else if(command_mnemonic == JA_M) command_code = JA;
                else if(command_mnemonic == JAE_M) command_code = JAE;
                else if(command_mnemonic == JB_M) command_code = JB;
                else command_code = JBE;

                read_word(&file_mapping, &offset_input, &word_size, line_number);
                if(word_size == 0) {
                    rc = EXPECTED_ARGUMENT;
                    goto EXIT_BLOCK;
                }
                BUFFER_WRITE_IF_NEEDED(COMMAND_SIZE)
                buffer_output[offset_output % BUFFER_SIZE] = command_code;
                offset_output += COMMAND_SIZE;

                tmp_label.name = file_mapping.program + offset_input;
                tmp_label.address = offset_output;
                rc = vector_push(labels_jmp, &tmp_label);
                if(rc != OK) goto EXIT_BLOCK;

                BUFFER_WRITE_IF_NEEDED(sizeof(address_t))
                for(size_t i = 0; i < sizeof(address_t); ++i) {
                    buffer_output[offset_output % BUFFER_SIZE] = 0;
                    ++offset_output;
                }
                break;
            case CALL_M:
                read_word(&file_mapping, &offset_input, &word_size, line_number);
                if(word_size == 0) {
                    rc = EXPECTED_ARGUMENT;
                    goto EXIT_BLOCK;
                }
                BUFFER_WRITE_IF_NEEDED(COMMAND_SIZE)
                buffer_output[offset_output % BUFFER_SIZE] = CALL;
                offset_output += COMMAND_SIZE;

                tmp_label.name = file_mapping.program + offset_input;
                tmp_label.address = offset_output;
                rc = vector_push(labels_jmp, &tmp_label);
                if(rc != OK) goto EXIT_BLOCK;

                BUFFER_WRITE_IF_NEEDED(sizeof(address_t))
                for(size_t i = 0; i < sizeof(address_t); ++i) {
                    buffer_output[offset_output % BUFFER_SIZE] = 0;
                    ++offset_output;
                }

            case RET_M:
                if(command_mnemonic == RET_M) {
                    BUFFER_WRITE_IF_NEEDED(COMMAND_SIZE)
                    buffer_output[offset_output % BUFFER_SIZE] = RET;
                    offset_output += COMMAND_SIZE;
                }

                read_word(&file_mapping, &offset_input, &word_size, line_number);
                if(word_size == 0) {
                    rc = EXPECTED_ARGUMENT;
                    goto EXIT_BLOCK;
                }
                end_ptr = NULL;
                size_t number_params = strtoul(file_mapping.program + offset_input, &end_ptr, 0);
                if(number_params == ULONG_MAX ||
                (number_params == 0 && end_ptr == file_mapping.program + offset_input)) {
                    rc = INVALID_ARGUMENT;
                    goto EXIT_BLOCK;
                }

                char* ptr_number_params = (char*)(&number_params);
                BUFFER_WRITE_IF_NEEDED(sizeof(size_t))
                for(size_t i = 0; i < sizeof(size_t); ++i) {
                    buffer_output[offset_output % BUFFER_SIZE] = ptr_number_params[i];
                    ++offset_output;
                }
                break;
            case STOP_M:
                BUFFER_WRITE_IF_NEEDED(COMMAND_SIZE)
                buffer_output[offset_output % BUFFER_SIZE] = STOP;
                offset_output += COMMAND_SIZE;
                break;
            case OUT_M:
                BUFFER_WRITE_IF_NEEDED(COMMAND_SIZE)
                buffer_output[offset_output % BUFFER_SIZE] = OUT;
                offset_output += COMMAND_SIZE;
                break;
            default:
                // if it is not label
                if(*(file_mapping.program + offset_input + word_size - 1) != ':') {
                    rc = UNRECOGNIZED_COMMAND;
                    goto EXIT_BLOCK;
                }
                for(size_t i = 0; i < labels_address->size; ++i) {
                    if(str_is_equal(file_mapping.program + offset_input, labels_address->data[i].name)) {
                        rc = LABEL_ALREADY_EXIST;
                        goto EXIT_BLOCK;
                    }
                }
                tmp_label.name = file_mapping.program + offset_input;
                tmp_label.address = offset_output;
                rc = vector_push(labels_address, &tmp_label);
                if(rc != OK) goto EXIT_BLOCK;
        }
        if(is_read_next_word)
            read_word(&file_mapping, &offset_input, &word_size, line_number);
    }

    number_elem = fwrite(buffer_output, sizeof(char), offset_output % BUFFER_SIZE, assemble_file);
    if(number_elem != offset_output % BUFFER_SIZE) {
        rc = ERROR_FILE_WRITE;
        goto EXIT_BLOCK;
    }
    // restoring addresses
    size_t index_label = SIZE_MAX;
    address_t address = 0;
    for(size_t i = 0; i < labels_jmp->size; ++i) {
        index_label = SIZE_MAX;
        for(size_t j = 0; j < labels_address->size; ++j) {
            if(str_is_equal(labels_jmp->data[i].name, labels_address->data[j].name)) {
                index_label = j;
                break;
            }
        }
        if(index_label == SIZE_MAX) {
            rc = LABEL_NOT_FOUND;
            goto EXIT_BLOCK;
        }
        offset_output = labels_jmp->data[i].address;
        address = labels_address->data[index_label].address - offset_output + COMMAND_SIZE;
        if(fseek(assemble_file, offset_output, SEEK_SET) != 0) {
            rc = ERROR_FILE_WRITE;
            goto EXIT_BLOCK;
        }
        number_elem = fwrite(&address, sizeof(address_t), 1, assemble_file);
        if(number_elem != 1) {
            rc = ERROR_FILE_WRITE;
            goto EXIT_BLOCK;
        }
    }

    EXIT_BLOCK:
    vector_destroy(&labels_address);
    vector_destroy(&labels_jmp);
    unmap_file(&file_mapping);
    fclose(assemble_file);
    return rc;
}

static int str_is_equal_i(const char* first, const char* second)
{
    assert(first != NULL);
    assert(second != NULL);

    size_t i = 0;
    char is_first_end = FALSE;
    char is_second_end = FALSE;
    char difference = 'a' - 'A';

    while(TRUE) {
        if(first[i] == '\0' || first[i] == ' ' || first[i] == '\n'
           || first[i] == ':' || first[i] == '\r')
            is_first_end = TRUE;
        if(second[i] == '\0' || second[i] == ' ' || second[i] == '\n'
           || second[i] == ':' || second[i] == '\r')
            is_second_end = TRUE;
        if(is_first_end && is_second_end)
            return TRUE;
        if(!(first[i] == second[i] || first[i] == (second[i] + difference)
             || (first[i] + difference) == second[i]) || is_first_end || is_second_end)
            return FALSE;
        ++i;
    }
}

static int str_is_equal(const char* first, const char* second)
{
    assert(first != NULL);
    assert(second != NULL);

    char is_first_end = FALSE;
    char is_second_end = FALSE;
    size_t i = 0;
    while(TRUE) {
        if(first[i] == '\0' || first[i] == ' ' || first[i] == '\n'
           || first[i] == ':' || first[i] == '\r')
            is_first_end = TRUE;
        if(second[i] == '\0' || second[i] == ' ' || second[i] == '\n'
           || second[i] == ':' || second[i] == '\r')
            is_second_end = TRUE;
        if(is_first_end && is_second_end)
            return TRUE;
        if(!(first[i] == second[i]) || is_first_end || is_second_end)
            return FALSE;
        ++i;
    }
}

static void read_word(const FileMapping* file_mapping, size_t* offset, size_t* size, size_t* line_number)
{
    assert(file_mapping != NULL);
    assert(offset != NULL);
    assert(size != NULL);
    assert(line_number != NULL);

    *offset += *size;
    *size = 0;
    while(file_mapping->size > *offset) {
        if(file_mapping->program[*offset] == '\n') {
            ++(*line_number);
            ++(*offset);
        }
        else
            if (file_mapping->program[*offset] == ' ' || file_mapping->program[*offset] == '\r')
                ++(*offset);
            else
                break;
    }

    while(file_mapping->size > *offset + *size && file_mapping->program[*offset + *size] != '\r'
    && file_mapping->program[*offset + *size] != '\n' && file_mapping->program[*offset + *size] != ' ') {
        ++(*size);
    }
}

