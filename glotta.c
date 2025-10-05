#define GLOTTA_IMPLEMENTATION
#include "glotta.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Please provide a directory to read\n");
        return 1;
    }

    printf("Getting stats\n");

    Vec *result = vec_new(sizeof(char *));
    if (!glotta_get_stats(result, argv[1])) {
        perror("main");
        return 1;
    }

    printf("Printing stats\n");
    vec_print(result);

    printf("Freeing stats\n");
    vec_free_deep(result);
    printf("Stats freed\n");

    return 0;
}
