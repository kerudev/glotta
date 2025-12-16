#ifndef GLOTTA_H
#define GLOTTA_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef __USE_MISC
#define __USE_MISC
#endif

#include <dirent.h>

#define STRUCTYPES_IMPLEMENTATION
#define STRUCTYPES_DEBUG
#include <structypes/hashmap.h>
#include <structypes/node.h>
#include <structypes/str.h>

// static HashMap *GLOTTA_LANGS = hashmap_new(HASHMAP_CAPACITY_STEP);

//  "C",        "c",
//  "C++",      "cpp",
//  "Rust",     "rs",
//  "Python",   "py",
//  "GLSL",     "vect",
//  "GLSL",     "frag",

bool glotta_get_stats(Node *files, char *path);

bool glotta_read_path(Node *files, char *path);

bool glotta_ignore_path(char *path);

// char *glotta_get_lang(char *lang);

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
            char *dirName = str_concat(path, slash, entry->d_name);

            glotta_read_path(node, dirName);
            free(dirName);
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

#endif // GLOTTA_IMPLEMENTATION
