#define GLOTTA_IMPLEMENTATION
#include "glotta.h"

#include <time.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("[ERR ]  Please provide a directory to read\n");
        return 1;
    }

    char *path = argv[1];

    Tree *result = tree_new(path);

    printf("[INFO]  Getting stats\n");
    if (!glotta_get_stats(result, path)) {
        printf("[ERR ]  Getting stats failed\n");
        return 1;
    }

    printf("\n");
    printf("[INFO]  Printing stats\n");
    if (!tree_print(result)) {
        printf("[ERR ]  Printing failed\n");
        return 1;
    }
    
    printf("\n");
    printf("[INFO]  Freeing stats\n");
    if (!tree_free(result)) {
        printf("[ERR ]  Freeing failed\n");
        return 1;
    }
    printf("[INFO]  Stats freed\n");

    return 0;
}
