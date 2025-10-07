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
#include <structypes/vec.h>
#include <structypes/str.h>

static const char* GLOTTA_LANGS[] = {
    "C",        "c",
    "C++",      "cpp",
    "Rust",     "rs",
    "Python",   "py",
    "GLSL",     "vect frag",
};

bool glotta_get_stats(Vec *files, char *path);

bool glotta_read_path(Vec *files, char *path);

bool glotta_ignore_path(char *path);

// char *glotta_get_lang(char *lang);

#endif  // GLOTTA_H

#ifdef GLOTTA_IMPLEMENTATION

int depth = 0;

void indent(int n) {
    for (size_t i = 0; i < n; i++) printf("  ");
}

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

bool glotta_read_path(Vec *files, char *path) {
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

            glotta_read_path(files, dirName);
            break;

        default:
            char *str = strdup(entry->d_name);
            vec_push(files, &str);
            break;
        }
    }

    closedir(dir);

    return true;
}

bool glotta_get_stats(Vec *files, char *path) {
    if (!glotta_read_path(files, path)) {
        perror("glotta_get_stats");
        return false;
    }

    return true;
}

#endif // GLOTTA_IMPLEMENTATION
