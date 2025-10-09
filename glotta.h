#ifndef GLOTTA_H
#define GLOTTA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#ifndef __USE_MISC
#define __USE_MISC
#endif

#include <dirent.h>

#define STRUCTYPES_IMPLEMENTATION
#include <structypes/tree.h>
#include <structypes/str.h>

static const char* GLOTTA_LANGS[] = {
    "C",        "c",
    "C++",      "cpp",
    "Rust",     "rs",
    "Python",   "py",
    "GLSL",     "vect frag",
};

bool glotta_get_stats(Tree *files, char *path);

bool glotta_read_path(Tree *files, char *path);

bool glotta_ignore_path(char *path);

// char *glotta_get_lang(char *lang);

#endif  // GLOTTA_H

#ifdef GLOTTA_IMPLEMENTATION

bool glotta_ignore_path(char *path) {
    char *ignored[] = {
        ".",
        "..",
    };

    int len = sizeof(ignored) / sizeof(ignored[0]);

    for (size_t i = 0; i < len; i++) {
        if (strcmp(ignored[i], path) == 0) return true;
    }

    return false;
}

bool glotta_read_path(Tree *files, char *path) {
    DIR *dir = opendir(path);
    if (dir == NULL) {
        printf("Could not open directory '%s'\n", path);
        return false;
    }

    struct dirent *entry;
    while ((entry = readdir(dir))) {
        if (glotta_ignore_path(entry->d_name)) continue;

        switch (entry->d_type) {
        case DT_DIR:
            char dirName[256];
            strcpy(dirName, path);

            if (str_char_at(dirName, -1) != '/')
                strcat(dirName, "/");

            strcat(dirName, entry->d_name);

            Node *node = node_new(files, strdup(dirName));
            glotta_read_path(node, node->value);
            node_add_child(files, node);
            break;

        default:
            node_new_child(files, strdup(entry->d_name));
            break;
        }
    }

    closedir(dir);

    return true;
}

bool glotta_get_stats(Tree *files, char *path) {
    if (!glotta_read_path(files, path)) {
        printf("glotta_get_stats: error while reading %s\n", path);
        return false;
    }

    return true;
}

#endif // GLOTTA_IMPLEMENTATION
