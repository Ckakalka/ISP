/**
 * @file hamlet_sort.h
 * The library for hamlet sort.
 */
#ifndef HAMLET_SORT_HAMLET_SORT_H
#define HAMLET_SORT_HAMLET_SORT_H
#include <sys/stat.h>
#include <sys/types.h>

int mmap_data(const char *path, char **text, struct stat *file_info);
int get_lines(char *text, off_t size, char ***start_lines, char ***end_lines, int *count_lines);

int quick_sort(void *start, size_t number, size_t elem_size, int (*comparator)(const void *, const void *));
static void choose_sort(void *start, size_t number, size_t elem_size, int (*comparator)(const void *, const void *));
static void recursive_sort(void *start, size_t number, size_t elem_size, int (*comparator)(const void *, const void *));
static void two_elem_sort(void *first, size_t elem_size, int (*comparator)(const void *, const void *));
static void three_elem_sort(void *first, size_t elem_size, int (*comparator)(const void *, const void *));
static void insert_sort(void *start, size_t number, size_t elem_size,  int (*comparator)(const void *, const void *));


void swap(void *first, void *second, size_t elem_size);


#endif //HAMLET_SORT_HAMLET_SORT_H
