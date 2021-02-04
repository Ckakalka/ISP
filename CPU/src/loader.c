/**
 * @file loader.c
 * Contains function implementation for file loading
 */
#include "loader.h"

/**
 * Maps a file \b path to memory and writes data about it to a \b file_mapping
 * @param path Path to the file
 * @param file_mapping The structure in which the information about the file map will be written
 * @return See return_codes.md
 */
int load_file(const char* path, FileMapping* file_mapping)
{
    if(path == NULL)
        return ARGUMENT_IS_NULL;
    if(file_mapping == NULL)
        return ARGUMENT_IS_NULL;

    HANDLE file = CreateFile(path, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if(file == INVALID_HANDLE_VALUE)
        return ERROR_FILE_OPEN;

    DWORD file_size = GetFileSize(file, 0);
    if(file_size == INVALID_FILE_SIZE) {
        CloseHandle(file);
        return ERROR_FILE_OPEN;
    }

    HANDLE mapping = CreateFileMapping(file, 0, PAGE_READWRITE, 0, 0, 0);
    if(mapping == NULL) {
        CloseHandle(file);
        return ERROR_FILE_OPEN;
    }

    const char* data = (const char*) MapViewOfFile(mapping, FILE_MAP_READ, 0, 0, 0);
    if(data == NULL) {
        CloseHandle(mapping);
        CloseHandle(file);
        return ERROR_FILE_OPEN;
    }

    file_mapping->file = file;
    file_mapping->mapping = mapping;
    file_mapping->size = file_size;
    file_mapping->program = data;

    return OK;
}

/**
 * Stops mapping file to memory
 * @param file_mapping Pointer to the structure with file mapping information
 * @return See return_codes.md
 */
int unmap_file(FileMapping* file_mapping)
{
    if(UnmapViewOfFile(file_mapping->program) == 0)
        return ERROR_FILE_CLOSE;
    if(CloseHandle(file_mapping->mapping) == 0)
        return ERROR_FILE_CLOSE;
    if(CloseHandle(file_mapping->file) == 0)
        return ERROR_FILE_CLOSE;
    return OK;
}
