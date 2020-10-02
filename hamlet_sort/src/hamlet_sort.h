/**
 * @file hamlet_sort.h
 * The library for hamlet sort.
 */
#ifndef HAMLET_SORT_HAMLET_SORT_H
#define HAMLET_SORT_HAMLET_SORT_H
#include <sys/stat.h>
#include <sys/types.h>


int hamlet_sort(const char *path);
static int mmap_data(const char *path, char **text, struct stat *file_info);
static int get_lines(char *text, off_t size, char ***start_lines, int *count_lines);

int quick_sort(void *start, size_t number, size_t elem_size, int (*comparator)(const void *, const void *));
static void choose_sort(void *start, size_t number, size_t elem_size, int (*comparator)(const void *, const void *));
static void recursive_sort(void *start, size_t number, size_t elem_size, int (*comparator)(const void *, const void *));
static void two_elem_sort(void *first, size_t elem_size, int (*comparator)(const void *, const void *));
static void three_elem_sort(void *first, size_t elem_size, int (*comparator)(const void *, const void *));
static void insert_sort(void *start, size_t number, size_t elem_size,  int (*comparator)(const void *, const void *));


void swap(void *first, void *second, size_t elem_size);

static int string_straight_comparator(const void *left, const void *right);
static int string_reverse_comparator(const void *left, const void *right);
static int is_letter(char symbol);
static int is_end_line(const void *address);
static void start_to_end_lines(char **start_lines, int count_lines);
static void end_to_start_lines(char **end_lines, int count_lines);
static int get_ord_symbol(char symbol);


#endif //HAMLET_SORT_HAMLET_SORT_H
