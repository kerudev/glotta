#define GLOTTA_IMPLEMENTATION
#include "glotta.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Please provide a directory to read\n");
        return 1;
    }
    
    Vec *result = vec_new(sizeof(char *) * 16);
    if (!glotta_get_stats(result, argv[1])) {
        perror("main");
        return 1;
    }

    return 0;
}
