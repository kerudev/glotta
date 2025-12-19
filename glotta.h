#ifndef GLOTTA_H
#define GLOTTA_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#ifndef __USE_MISC
#define __USE_MISC
#endif

#include <dirent.h>

#define STRUCTYPES_IMPLEMENTATION
#define STRUCTYPES_DEBUG
#include <structypes/hashmap.h>
#include <structypes/node.h>
#include <structypes/str.h>

bool glotta_get_stats(Node *files, char *path);

bool glotta_read_path(Node *files, char *path);

bool glotta_ignore_path(char *path);

bool glotta_init_langs();

static HashMap *GLOTTA_LANGS = NULL;

#endif  // GLOTTA_H

#ifdef GLOTTA_IMPLEMENTATION

bool glotta_ignore_path(char *path) {
    char *ignored[] = {
        ".",
        "..",
    };

    size_t len = sizeof(ignored) / sizeof(ignored[0]);

    for (size_t i = 0; i < len; i++) {
        if (str_eq(ignored[i], path)) return true;
    }

    return false;
}

bool glotta_read_path(Node *files, char *path) {
    DIR *dir = opendir(path);
    if (dir == NULL) {
        printf("Could not open directory '%s'\n", path);
        return false;
    }

    struct dirent *entry;
    while ((entry = readdir(dir))) {
        if (glotta_ignore_path(entry->d_name)) continue;

        Node *node = node_add(files, str_clone(entry->d_name));

        if (entry->d_type == DT_DIR) {
            char *slash = (str_char_at(path, -1) != '/') ? "/" : "";
            char *dir_name = str_concat(path, slash, entry->d_name);

            glotta_read_path(node, dir_name);
            free(dir_name);
        }
    }

    closedir(dir);

    return true;
}

bool glotta_get_stats(Node *files, char *path) {
    if (!glotta_read_path(files, path)) {
        printf("glotta_get_stats: error while reading %s\n", path);
        return false;
    }

    return true;
}

bool glotta_print_path(char *path) {
    Node *result = node_new(str_clone(path));

    printf("[INFO]  Getting stats\n");
    if (!glotta_get_stats(result, path)) {
        printf("[ERR ]  Getting stats failed\n");
        return false;
    }

    printf("\n");
    printf("[INFO]  Printing stats\n");
    if (!node_print(result, nodeprintopts_default())) {
        printf("[ERR ]  Printing failed\n");
        return false;
    }

    printf("\n");
    printf("[INFO]  Freeing stats\n");
    if (!node_free(result)) {
        printf("[ERR ]  Freeing failed\n");
        return false;
    }

    printf("[INFO]  Stats freed\n");

    return true;
}

#endif // GLOTTA_IMPLEMENTATION
