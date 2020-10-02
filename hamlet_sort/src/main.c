#include <stdio.h>
#include "hamlet_sort.h"
#include <sys/mman.h>
#include "malloc.h"


int main()
{
        char *text = NULL;
        struct stat file_info;
        int rez = mmap_data("../../src/hamlet.txt", &text, &file_info);
        if (rez == -1) {
                printf("Open or map error\n");
                return -1;
        }

        char **start_lines = NULL;
        int count_lines = 0;
        rez = get_lines(text, file_info.st_size, &start_lines, &count_lines);

        if (rez == -1) {
                printf("get_lines error\n");
                return -1;
        }

        quick_sort(start_lines, count_lines, sizeof(start_lines), &string_straight_comparator);

        for (int i = 0; i < count_lines; ++i) {
                printf("%s\n", start_lines[i]);
        }

        if (munmap((void *)text, file_info.st_size) == -1) {
                printf("Unmap error\n");
                return -1;
        }

        free(start_lines);
        return 0;
}

