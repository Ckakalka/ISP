#include <stdio.h>
#include "hamlet_sort.h"
#include <sys/mman.h>
#include "malloc.h"






int comparator(const void *left, const void *right)
{
        if ((*(int *)left) > (*(int *) right))
                return 1;
        else if ((*(int *) left) == (*(int *) right))
                return 0;
        return -1;
}


int main()
{
        char *text = NULL;
        struct stat file_info;
        int rez = mmap_data("../../src/temp", &text, &file_info);
        if (rez == -1) {
                printf("Open or map error\n");
                return -1;
        }

//        for (int i = 0; i < file_info.st_size; ++i)
//                printf("%d\n", text[i]);

        char **start_lines = NULL;
        char **end_lines   = NULL;
        int count_lines = 0;
        rez = get_lines(text, file_info.st_size, &start_lines, &end_lines, &count_lines);

        if (rez == -1) {
                printf("get_string error\n");
                return -1;
        }

//        for (int i = 0; i < count_lines; ++i) {
//                char *tmp = start_lines[i];
//                while (tmp < end_lines[i]) {
//                        printf("%c", *tmp);
//                        ++tmp;
//                }
//                printf("\n");
//        }
        printf("%s", text);


        if (munmap((void *)text, file_info.st_size) == -1) {
                printf("Unmap error\n");
                return -1;
        }

        free(start_lines);
        free(end_lines);

        /*int n = 3;
//        scanf("%d", &n);
        int *arr = calloc(n, sizeof(*arr));

        for (int i = 0; i < n; ++i)
                scanf("%d", &(arr[i]));

//        printf("Array\n");
//        for (int i = 0; i < n; ++i)
//                printf("%d ", arr[i]);

        int rez = quick_sort(arr, n, sizeof(*arr), NULL);
        for (int i = 0; i < n; ++i)
                printf("%d ", arr[i]);

        free(arr);
        printf("%d", rez);*/
        return 0;
}

