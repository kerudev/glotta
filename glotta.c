#define GLOTTA_IMPLEMENTATION
#include "glotta.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <path> <-p|-l|-s|-c>\n", argv[0]);
        printf("-p: print contents of a directory in tree form.\n");
        printf("-l: count the lines of each file by type.\n");
        printf("-s: count the size of each file by type.\n");
        printf("-t: count total files of each type.\n");
        return 0;
    }

    char *path = argv[1];
    char *arg = argv[2];

    printf("[INFO]  Path: %s\n", path);
    printf("[INFO]  Args: %s\n", arg);

    if (str_eq(arg, "-p")) return !glotta_print_path(path);
    if (str_eq(arg, "-l")) return !glotta_print_lines(path);
    if (str_eq(arg, "-s")) return !glotta_print_size(path);
    if (str_eq(arg, "-t")) return !glotta_print_count(path);

    printf("[ERR ]  Unknown argument %s\n", arg);

    return 1;
}
