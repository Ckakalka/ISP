/**
 * @file loader.h
 * Contains function headers for file loading
 */
#ifndef CPU_LOADER_H
#define CPU_LOADER_H

#include "structs_and_enums.h"

int load_file(const char* path, FileMapping* file_mapping);
int unmap_file(FileMapping* file_mapping);

#endif //CPU_LOADER_H
