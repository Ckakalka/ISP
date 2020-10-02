#include "hamlet_sort.h"
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <assert.h>
#include <malloc.h>

/**
 * Pointer to the beginning of the file.
 */
static void *begin_file;
/**
 * Pointer to the end of the file.
 */
static void *end_file;

/**
 * Sorts text file \b path line by line. First in straight order, then in reverse. Ignores all characters except Latin.
 * @param path The path to the file.
 * @return Returns 0 on success and -1 on failure.
 */
int hamlet_sort(const char *path)
{
        assert(path != NULL);
        if (path == NULL)
                return -1;

        char *text = NULL;
        struct stat file_info;
        int rez = mmap_data(path, &text, &file_info);
        if (rez == -1) {
                fprintf(stderr, "Open or map error\n");
                return -1;
        }

        char **start_lines = NULL;
        int count_lines = 0;
        rez = get_lines(text, file_info.st_size, &start_lines, &count_lines);

        if (rez == -1) {
                fprintf(stderr, "get_lines error\n");
                return -1;
        }

        quick_sort(start_lines, count_lines, sizeof(start_lines), &string_straight_comparator);
        start_to_end_lines(start_lines, count_lines);
        quick_sort(start_lines, count_lines, sizeof(start_lines), &string_reverse_comparator);
        end_to_start_lines(start_lines, count_lines);

        for (int i = 0; i < count_lines; ++i) {
                if (*(start_lines[i]) != '\0')
                        printf("%s\n", start_lines[i]);
        }

        for(off_t i = 0; i < file_info.st_size; ++i)
                if(text[i] == '\0')
                        text[i] = '\n';

        printf("Original\n");

        printf("%s", text);

        if (munmap((void *)text, file_info.st_size) == -1) {
                fprintf(stderr, "Unmap error\n");
                return -1;
        }

        free(start_lines);
        return 0;
}

/**
 * Maps the file at path \b path to memory. Sets \b text to the beginning of the file.
 * Puts file data at address \b file_info.
 * @param [in] path  The path to the file (absolute or relative).
 * @param [out] text Pointer to the string (not NULL).
 * @param [out] file_info Pointer to the struct stat (not NULL).
 * @return Returns 0 on success and -1 on failure.
 */
static int mmap_data(const char *path, char **text, struct stat *file_info)
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
* Marks lines of \b text. Each line corresponds to two pointers: to the first symbol of line.
* This pointer is placed in address \b start_lines. Also puts the count of lines in address \b count_lines.
* Replaces \\n with \\0.
* @param [in] text The text in which the lines are marked (not NULL).
* @param [in] size The size of the \b text.
* @param [out] start_lines Address of the array of the pointer to the char that is the start of the line.
* @param [out] count_lines Address to which the count of lines will be written.
* @return Returns 0 on success and -1 on failure.
 * \warning Dynamic memory is allocated for the \b start_lines. Do not forget to free it.
*/
static int get_lines(char *text, off_t size, char ***start_lines, int *count_lines)
{
        assert(text != NULL);
        assert(count_lines != NULL);
        assert(start_lines != NULL);
        if (text == NULL || count_lines == NULL ||
                start_lines == NULL)
                return -1;

        if (size == 0)
                return 0;

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

        **start_lines = text;
        if (*text == '\n')
                *text = '\0';

        *count_lines = 1;
        byte_counter = 1;

        off_t size_without_last = size - 1;
        while (byte_counter < size_without_last) {
                if (text[byte_counter] == '\n') {
                        text[byte_counter] = '\0';
                        (*start_lines)[*count_lines] = &(text[byte_counter + sizeof(char)]);
                        ++(*count_lines);
                }
                ++byte_counter;
        }

        if (text[size_without_last] == '\n')
                text[size_without_last] = '\0';
        begin_file = text;
        end_file = &(text[size_without_last]);
        return 0;
}

/**
 * Sorting the \b number elements length \b elem_size of array from the address \b start by rule \b comparator.
 * @param start The address from which sorting is performed.
 * @param number Number of elements to sort.
 * @param elem_size The size of the element (in bytes).
 * @param comparator Function to compare elements.
 * @return Returns 0 on success and -1 on failure.
 */
int quick_sort(void *start, size_t number, size_t elem_size, int (*comparator)(const void *, const void *))
{
        assert(start != NULL);
        assert(comparator != NULL);
        if (start == NULL || comparator == NULL)
                return -1;
        choose_sort(start, number, elem_size, comparator);
        return 0;
}

/**
 * Byte by byte changes the values at addresses \b first and \b second.
 * @param first Address of the first element.
 * @param second Address of the second element.
 * @param elem_size The size of the element (in bytes).
 */
void swap(void *first, void *second, size_t elem_size)
{
        char tmp;
        for (size_t i = 0; i < elem_size; ++i) {
                tmp = *((char *)first + i);
                *((char *)first + i) = *((char *)second + i);
                *((char*)second + i) = tmp;
        }
}

static int string_straight_comparator(const void *left, const void *right)
{
        char *left_symbol  = *(char **)left;
        char *right_symbol = *(char **)right;
        while (1) {
                while (!(is_end_line(left_symbol) || is_letter(*left_symbol)))
                        left_symbol += sizeof(char);
                while (!(is_end_line(right_symbol) || is_letter(*right_symbol)))
                        right_symbol += sizeof(char);

                if (is_end_line(left_symbol) && is_end_line(right_symbol))
                        return 0;
                if (!is_end_line(left_symbol) && is_end_line(right_symbol))
                        return 1;
                if (is_end_line(left_symbol) && !is_end_line(right_symbol))
                        return -1;

                if (get_ord_symbol(*left_symbol) > get_ord_symbol(*right_symbol))
                        return 1;
                if (get_ord_symbol(*left_symbol) < get_ord_symbol(*right_symbol))
                        return -1;
                left_symbol  += sizeof(char);
                right_symbol += sizeof(char);
        }
}

static int get_ord_symbol(char symbol)
{
        if (symbol >= 'A' && symbol <= 'Z')
                return symbol - 'A';
        return symbol - 'a';
}

static int is_end_line(const void *address)
{
        return *(char *)address == '\0' || address > end_file;
}

static int is_start_line(const  void *address)
{
        return *(char *)address == '\0' || address < begin_file;
}

static int string_reverse_comparator(const void *left, const void *right)
{
        char *left_symbol  = *(char **)left;
        char *right_symbol = *(char **)right;
        while (1) {
                while (!(is_start_line(left_symbol) || is_letter(*left_symbol)))
                        left_symbol -= sizeof(char);
                while (!(is_start_line(right_symbol) || is_letter(*right_symbol)))
                        right_symbol -= sizeof(char);

                if (is_start_line(left_symbol) && is_start_line(right_symbol))
                        return 0;
                if (!is_start_line(left_symbol) && is_start_line(right_symbol))
                        return 1;
                if (is_start_line(left_symbol) && !is_start_line(right_symbol))
                        return -1;

                if (get_ord_symbol(*left_symbol) > get_ord_symbol(*right_symbol))
                        return 1;
                if (get_ord_symbol(*left_symbol) < get_ord_symbol(*right_symbol))
                        return -1;
                left_symbol  -= sizeof(char);
                right_symbol -= sizeof(char);
        }
}

static void start_to_end_lines(char **start_lines, int count_lines)
{
        for (int i = 0; i < count_lines; ++i)
                if (*(start_lines[i]) != '\0') {
                        while(!(start_lines[i] > (char *)end_file || *(start_lines[i]) == '\0'))
                                start_lines[i] += sizeof(char);
                        start_lines[i] -= sizeof(char);
                }
}

static void end_to_start_lines(char **end_lines, int count_lines)
{
        for (int i = 0; i < count_lines; ++i)
                if (*(end_lines[i]) != '\0') {
                        while(!(end_lines[i] < (char *)begin_file || *(end_lines[i]) == '\0'))
                                end_lines[i] -= sizeof(char);
                        end_lines[i] += sizeof(char);
                }
}

static int is_letter(char symbol)
{
        return (('A' <= symbol && symbol <= 'Z')
             || ('a' <= symbol && symbol <= 'z'));
}

static void choose_sort(void *start, size_t number, size_t elem_size, int (*comparator)(const void *, const void *))
{
        assert(start != NULL);
        assert(comparator != NULL);

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
        assert(start != NULL);
        assert(comparator != NULL);

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
        assert(first != NULL);
        assert(comparator != NULL);

        void *second = first + elem_size;
        if (comparator(first, second) == 1)
                swap(first, second, elem_size);
}

static void three_elem_sort(void *first, size_t elem_size, int (*comparator)(const void *, const void *))
{
        assert(first != NULL);
        assert(comparator != NULL);

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
        assert(start != NULL);
        assert(comparator != NULL);

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




