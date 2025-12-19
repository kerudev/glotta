#define GLOTTA_IMPLEMENTATION
#include "glotta.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <path> <-p>\n", argv[0]);
        printf("-p: print contents of a directory in tree form.\n");
        return 0;
    }

    char *path = argv[1];
    char *arg = argv[2];

    printf("[INFO]  Path: %s\n", path);
    printf("[INFO]  Args: %s\n", arg);

    if (str_eq(arg, "-p") && !glotta_print_path(path)) return 1;

    return 0;
}
