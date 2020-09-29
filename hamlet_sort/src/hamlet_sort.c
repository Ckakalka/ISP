#include "hamlet_sort.h"
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <assert.h>
#include <malloc.h>

/**
 * Maps the file at path \b path to memory. Sets \b text to the beginning of the file.
 * Puts file data at address \b file_info.
 * @param [in] path  The path to the file (absolute or relative).
 * @param [out] text Pointer to the string (not NULL).
 * @param [out] file_info Pointer to the struct stat (not NULL).
 * @return Returns 0 on success and -1 on failure.
 */
int mmap_data(const char *path, char **text, struct stat *file_info)
{
        assert(path != NULL);
        assert(text != NULL);
        assert(file_info != NULL);
        if (path == NULL || text == NULL || file_info == NULL)
                return -1;


        int fd = open(path, O_RDWR);

        if (fd == -1)
                return -1;

        if (fstat(fd, file_info) == -1)
                return -1;

        *text = mmap(NULL, file_info->st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
        if (*text == MAP_FAILED)
                return -1;

        if (close(fd) == -1)
                return -1;
        return 0;
}

/**
 *
 * @param text
 * @param size
 * @param start_lines
 * @param end_lines
 * @param count_lines
 * @return
 */
int get_lines(char *text, off_t size, char ***start_lines, char ***end_lines, int *count_lines)
{
        assert(text != NULL);
        assert(count_lines != NULL);
        assert(start_lines != NULL);
        assert(end_lines != NULL);
        if (text == NULL || count_lines == NULL ||
                start_lines == NULL || end_lines == NULL)
                return -1;

        *count_lines = 0;
        off_t byte_counter = 0;

        while (byte_counter < size) {
                if (text[byte_counter] == '\n')
                        ++(*count_lines);
                ++byte_counter;
        }
        if (text[byte_counter - 1] != '\n')
                ++(*count_lines);


        *start_lines = calloc(*count_lines, sizeof(**start_lines));
        *end_lines   = calloc(*count_lines, sizeof(**end_lines));

        *count_lines = 0;
        byte_counter = 0;

        while (byte_counter < size) {
                if (text[byte_counter] == '\n') {
                        (*start_lines)[*count_lines] = ((*end_lines)[*count_lines - 1]) + sizeof(***start_lines);
                        (*end_lines)[*count_lines]   = &(text[byte_counter]);
                        ++(*count_lines);
                }
                ++byte_counter;
        }

        **start_lines = text;
        if (text[byte_counter - 1] != '\n') {
                (*start_lines)[*count_lines] = ((*end_lines)[*count_lines - 1]) + sizeof(***start_lines);
                (*end_lines)[*count_lines]   = &(text[byte_counter]);
                ++(*count_lines);
        }
        return 0;
}

int quick_sort(void *start, size_t number, size_t elem_size, int (*comparator)(const void *, const void *))
{
        assert(start != NULL);
        assert(comparator != NULL);
        if (start == NULL || comparator == NULL)
                return -1;
        choose_sort(start, number, elem_size, comparator);
        return 0;
}

static void choose_sort(void *start, size_t number, size_t elem_size, int (*comparator)(const void *, const void *))
{
        assert(start == NULL);
        assert(comparator);

        if (number < 2)
                return;

        if (number == 2) {
                two_elem_sort(start, elem_size, comparator);
                return;
        }

        if (number == 3) {
                three_elem_sort(start, elem_size, comparator);
                return;
        }

        if (number <= 16) {
                insert_sort(start, number, elem_size, comparator);
                return;
        }
        recursive_sort(start, number, elem_size, comparator);
}

static void recursive_sort(void *start, size_t number, size_t elem_size, int (*comparator)(const void *, const void *))
{
        assert(start == NULL);
        assert(comparator);

        void *end  = start + elem_size * (number - 1);
        void *left = start;
        void *right = end;
        char pivot[elem_size];
        for(size_t i = 0; i < elem_size; ++i)
                pivot[i] = *(char *)(start + elem_size * (number / 2) + i);

        while (left <= right) {

                // while *left <= *pivot
                while (comparator(left, pivot) == -1 && left <= right)
                        left += elem_size;

                // while *right > *pivot
                while (comparator(right, pivot) == 1 && right >= left)
                        right -= elem_size;

                if (left <= right) {
                        swap(left, right, elem_size);
                        left  += elem_size;
                        right -= elem_size;
                }
        }
        // left subarray
        if (right > start) {
                number = (right - start) / elem_size + 1;
                choose_sort(start, number, elem_size, comparator);
        }

        // right subarray
        if (end > left) {
                number = (end - left) / elem_size + 1;
                choose_sort(left, number, elem_size, comparator);
        }
}

static void two_elem_sort(void *first, size_t elem_size, int (*comparator)(const void *, const void *))
{
        assert(first == NULL);
        assert(comparator);

        void *second = first + elem_size;
        if (comparator(first, second) == 1)
                swap(first, second, elem_size);
}

static void three_elem_sort(void *first, size_t elem_size, int (*comparator)(const void *, const void *))
{
        assert(first == NULL);
        assert(comparator);

        void *second = first + elem_size;
        void *third  = second + elem_size;

        if (comparator(first, second) == 1) {
                if (comparator(second, third) == 1)
                        swap(first, third, elem_size);
                else {
                        swap(first, second, elem_size);
                        if (comparator(second, third) == 1)
                                swap(second, third, elem_size);
                }
        } else if (comparator(second, third) == 1) {
                swap(second, third, elem_size);
                if (comparator(first, second) == 1)
                        swap(first, second, elem_size);
        }
}

static void insert_sort(void *start, size_t number, size_t elem_size,  int (*comparator)(const void *, const void *))
{
        assert(start == NULL);
        assert(comparator);

        void *end = start + (number - 1) * elem_size;
        void *ptr_i = start + elem_size;
        void *ptr_j = NULL;
        while (ptr_i <= end) {
                ptr_j = ptr_i;
                while (ptr_j > start && comparator(ptr_j - elem_size, ptr_j) == 1) {
                        swap(ptr_j - elem_size, ptr_j, elem_size);
                        ptr_j -= elem_size;
                }
                ptr_i += elem_size;
        }
}

void swap(void *first, void *second, size_t elem_size)
{
        char tmp;
        for (size_t i = 0; i < elem_size; ++i) {
                tmp = *((char *)first + i);
                *((char *)first + i) = *((char *)second + i);
                *((char*)second + i) = tmp;
        }
}





