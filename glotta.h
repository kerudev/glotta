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

bool glotta_read_path(Node *files, char *path);

bool glotta_read_lines(HashMap *lines, char *path);

bool glotta_read_size(HashMap *lines, char *path);

bool glotta_read_count(HashMap *lines, char *path);

bool glotta_print_path(char *path);

bool glotta_print_lines(char *path);

bool glotta_print_size(char *path);

bool glotta_print_count(char *path);

bool glotta_ignore_path(char *path);

bool glotta_init_langs();

static HashMap *GLOTTA_LANGS = NULL;

#endif  // GLOTTA_H

#ifdef GLOTTA_IMPLEMENTATION

bool glotta_ignore_path(char *path) {
    // TODO read .gitignores
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
        printf("[ERR ]  Could not open directory '%s'\n", path);
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

bool glotta_read_lines(HashMap *lines, char *path) {
    // TODO handle case where path is not dir (count lines as well)
    DIR *dir = opendir(path);
    if (dir == NULL) {
        printf("[ERR ]  Could not open directory '%s'\n", path);
        return false;
    }

    // TODO handle case where extension is not in list
    struct dirent *entry;
    while ((entry = readdir(dir))) {
        if (glotta_ignore_path(entry->d_name)) continue;

        if (entry->d_type == DT_DIR) {
            char *slash = (str_char_at(path, -1) != '/') ? "/" : "";
            char *dir_name = str_concat(path, slash, entry->d_name);

            glotta_read_lines(lines, dir_name);
            free(dir_name);

            continue;
        }

        char *ext = str_split_last(entry->d_name, ".");

        void *value = hashmap_get(lines, ext);
        if (value == NULL) hashmap_set(lines, ext, 0);

        int count = value ? (int)(intptr_t)value : 0;

        char *file_name = str_join("/", path, entry->d_name);
        FILE *f = fopen(file_name, "r");
        free(file_name);

        for (char c = getc(f); c != EOF; c = getc(f)) {
            if (c == '\n') count += 1;
        }

        fclose(f);

        hash_t hash = hashmap_hash(ext, lines->capacity);
        lines->items[hash]->value = (void*)(intptr_t)count;
    }

    closedir(dir);

    return true;
}

bool glotta_read_size(HashMap *sizes, char *path) {
    // TODO handle case where path is not dir (count size as well)
    DIR *dir = opendir(path);

    if (dir == NULL) {
        printf("[ERR ]  Could not open directory '%s'\n", path);
        return false;
    }

    // TODO handle case where extension is not in list
    struct dirent *entry;
    while ((entry = readdir(dir))) {
        if (glotta_ignore_path(entry->d_name)) continue;

        if (entry->d_type == DT_DIR) {
            char *slash = (str_char_at(path, -1) != '/') ? "/" : "";
            char *dir_name = str_concat(path, slash, entry->d_name);

            glotta_read_size(sizes, dir_name);
            free(dir_name);

            continue;
        }

        char *ext = str_split_last(entry->d_name, ".");

        void *value = hashmap_get(sizes, ext);
        if (value == NULL) hashmap_set(sizes, ext, 0);

        long count = value ? (long)value : 0;

        char *file_name = str_join("/", path, entry->d_name);
        FILE *f = fopen(file_name, "r");

        fseek(f, 0, SEEK_END);
        count += ftell(f);

        fclose(f);
        free(file_name);

        hash_t hash = hashmap_hash(ext, sizes->capacity);
        sizes->items[hash]->value = (void*)count;
    }

    closedir(dir);

    return true;
}

bool glotta_read_count(HashMap *counts, char *path) {
    // TODO handle case where path is not dir (count size as well)
    DIR *dir = opendir(path);

    if (dir == NULL) {
        printf("[ERR ]  Could not open directory '%s'\n", path);
        return false;
    }

    // TODO handle case where extension is not in list
    struct dirent *entry;
    while ((entry = readdir(dir))) {
        if (glotta_ignore_path(entry->d_name)) continue;

        if (entry->d_type == DT_DIR) {
            char *slash = (str_char_at(path, -1) != '/') ? "/" : "";
            char *dir_name = str_concat(path, slash, entry->d_name);

            glotta_read_count(counts, dir_name);
            free(dir_name);

            continue;
        }

        char *ext = str_split_last(entry->d_name, ".");

        void *value = hashmap_get(counts, ext);
        if (value == NULL) hashmap_set(counts, ext, 0);

        int count = value ? (int)(intptr_t)value : 0;
        count++;

        hash_t hash = hashmap_hash(ext, counts->capacity);
        counts->items[hash]->value = (void*)(intptr_t)count;
    }

    closedir(dir);

    return true;
}

bool glotta_print_path(char *path) {
    Node *result = node_new(str_clone(path));

    printf("[INFO]  Getting stats\n");
    if (!glotta_read_path(result, path)) {
        printf("[ERR ]  Error while reading %s\n", path);
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

bool glotta_print_lines(char *path) {
    HashMap *result = hashmap_new(HASHMAP_CAPACITY_STEP);

    printf("[INFO]  Initializing languages hashmap\n");
    glotta_init_langs();

    printf("[INFO]  Getting stats\n");
    if (!glotta_read_lines(result, path)) goto defer;

    printf("\n");
    printf("[INFO]  Printing stats\n");

    printf("%s\n", path);
    for (size_t i = 0; i < result->capacity; i++) {
        if (!result->items[i]) continue;
        printf("- %s: %d\n", result->items[i]->key, (int)(intptr_t)result->items[i]->value);
    }

    goto defer;

defer:
    printf("\n");
    printf("[INFO]  Freeing stats\n");
    for (size_t i = 0; i < result->capacity; i++) {
        if (!result->size) break;
        if (!result->items[i]) continue;

        free(result->items[i]->key);
        free(result->items[i]);

        result->size--;
    }

    if (!hashmap_free_struct(result)) {
        printf("[ERR ]  Freeing struct failed\n");
        return false;
    }

    printf("[INFO]  Freeing langs\n");
    if (!hashmap_free_stack(GLOTTA_LANGS)) {
        printf("[ERR ]  Freeing langs failed\n");
        return false;
    }

    printf("[INFO]  Stats freed\n");

    return true;
}

bool glotta_print_size(char *path) {
    HashMap *result = hashmap_new(HASHMAP_CAPACITY_STEP);

    printf("[INFO]  Initializing languages hashmap\n");
    glotta_init_langs();

    printf("[INFO]  Getting stats\n");
    if (!glotta_read_size(result, path)) goto defer;

    printf("\n");
    printf("[INFO]  Printing stats\n");

    printf("%s\n", path);
    for (size_t i = 0; i < result->capacity; i++) {
        if (!result->items[i]) continue;
        printf("- %s: %d\n", result->items[i]->key, (int)(intptr_t)result->items[i]->value);
    }

    goto defer;

defer:
    printf("\n");
    printf("[INFO]  Freeing stats\n");
    for (size_t i = 0; i < result->capacity; i++) {
        if (!result->size) break;
        if (!result->items[i]) continue;

        free(result->items[i]->key);
        free(result->items[i]);

        result->size--;
    }

    if (!hashmap_free_struct(result)) {
        printf("[ERR ]  Freeing struct failed\n");
        return false;
    }

    printf("[INFO]  Freeing langs\n");
    if (!hashmap_free_stack(GLOTTA_LANGS)) {
        printf("[ERR ]  Freeing langs failed\n");
        return false;
    }

    printf("[INFO]  Stats freed\n");

    return true;
}

bool glotta_print_count(char *path) {
    HashMap *result = hashmap_new(HASHMAP_CAPACITY_STEP);

    printf("[INFO]  Initializing languages hashmap\n");
    glotta_init_langs();

    printf("[INFO]  Getting stats\n");
    if (!glotta_read_count(result, path)) goto defer;

    printf("\n");
    printf("[INFO]  Printing stats\n");

    printf("%s\n", path);
    for (size_t i = 0; i < result->capacity; i++) {
        if (!result->items[i]) continue;
        printf("- %s: %d\n", result->items[i]->key, (int)(intptr_t)result->items[i]->value);
    }

    goto defer;

defer:
    printf("\n");
    printf("[INFO]  Freeing stats\n");
    for (size_t i = 0; i < result->capacity; i++) {
        if (!result->size) break;
        if (!result->items[i]) continue;

        free(result->items[i]->key);
        free(result->items[i]);

        result->size--;
    }

    if (!hashmap_free_struct(result)) {
        printf("[ERR ]  Freeing struct failed\n");
        return false;
    }

    printf("[INFO]  Freeing langs\n");
    if (!hashmap_free_stack(GLOTTA_LANGS)) {
        printf("[ERR ]  Freeing langs failed\n");
        return false;
    }

    printf("[INFO]  Stats freed\n");

    return true;
}

bool glotta_init_langs() {
    if (GLOTTA_LANGS && GLOTTA_LANGS->size) return true;
    
    GLOTTA_LANGS = hashmap_new(HASHMAP_CAPACITY_STEP * 2);
    if (GLOTTA_LANGS) return false;

    // Low level
    hashmap_set(GLOTTA_LANGS, "asm",        "Assembly");
    hashmap_set(GLOTTA_LANGS, "nasm",       "Assembly");
    hashmap_set(GLOTTA_LANGS, "s",          "Assembly");

    hashmap_set(GLOTTA_LANGS, "c",          "C code");
    hashmap_set(GLOTTA_LANGS, "h",          "C header");

    hashmap_set(GLOTTA_LANGS, "c++",        "C++ code");
    hashmap_set(GLOTTA_LANGS, "cpp",        "C++ code");
    hashmap_set(GLOTTA_LANGS, "hpp",        "C++ header");

    hashmap_set(GLOTTA_LANGS, "ino",        "Arduino");

    hashmap_set(GLOTTA_LANGS, "rs",         "Rust");
    hashmap_set(GLOTTA_LANGS, "zig",        "Zig");
    hashmap_set(GLOTTA_LANGS, "d",          "D language");

    // Build tools
    hashmap_set(GLOTTA_LANGS, "Makefile",   "Make");
    hashmap_set(GLOTTA_LANGS, "cmake",      "CMake");
    hashmap_set(GLOTTA_LANGS, "ninja",      "Ninja");
    hashmap_set(GLOTTA_LANGS, "meson",      "Meson");

    // OpenGL
    hashmap_set(GLOTTA_LANGS, "vect",       "GLSL Vertex Shader");
    hashmap_set(GLOTTA_LANGS, "vert",       "GLSL Vertex Shader");
    hashmap_set(GLOTTA_LANGS, "vsh",        "GLSL Vertex Shader");
    hashmap_set(GLOTTA_LANGS, "vshader",    "GLSL Vertex Shader");
    hashmap_set(GLOTTA_LANGS, "vrx",        "GLSL Vertex Shader");
    hashmap_set(GLOTTA_LANGS, "glslv",      "GLSL Vertex Shader");

    hashmap_set(GLOTTA_LANGS, "frag",       "GLSL Fragment Shader");
    hashmap_set(GLOTTA_LANGS, "fs",         "GLSL Fragment Shader");
    hashmap_set(GLOTTA_LANGS, "fp",         "GLSL Fragment Shader");
    hashmap_set(GLOTTA_LANGS, "fsh",        "GLSL Fragment Shader");
    hashmap_set(GLOTTA_LANGS, "fshader",    "GLSL Fragment Shader");
    hashmap_set(GLOTTA_LANGS, "frg",        "GLSL Fragment Shader");

    hashmap_set(GLOTTA_LANGS, "geo",        "GLSL Geometry Shader");
    hashmap_set(GLOTTA_LANGS, "geom",       "GLSL Geometry Shader");
    hashmap_set(GLOTTA_LANGS, "gshader",    "GLSL Geometry Shader");

    hashmap_set(GLOTTA_LANGS, "glsl",       "GLSL");
    hashmap_set(GLOTTA_LANGS, "shader",     "GLSL");

    // High level
    hashmap_set(GLOTTA_LANGS, "py",         "Python");

    hashmap_set(GLOTTA_LANGS, "kt",         "Kotlin");
    hashmap_set(GLOTTA_LANGS, "java",       "Java");
    hashmap_set(GLOTTA_LANGS, "class",      "Java byteclass");

    hashmap_set(GLOTTA_LANGS, "go",         "Go");
    hashmap_set(GLOTTA_LANGS, "php",        "PHP");
    hashmap_set(GLOTTA_LANGS, "lua",        "Lua");
    hashmap_set(GLOTTA_LANGS, "rb",         "Ruby");
    hashmap_set(GLOTTA_LANGS, "cs",         "C#");

    // Frontend
    hashmap_set(GLOTTA_LANGS, "html",       "HTML");
    hashmap_set(GLOTTA_LANGS, "htm",        "HTML");

    hashmap_set(GLOTTA_LANGS, "css",        "CSS");
    hashmap_set(GLOTTA_LANGS, "scss",       "SCSS");
    hashmap_set(GLOTTA_LANGS, "sass",       "Sass");
    hashmap_set(GLOTTA_LANGS, "less",       "LESS");

    hashmap_set(GLOTTA_LANGS, "js",         "JavaScript");
    hashmap_set(GLOTTA_LANGS, "mjs",        "JavaScript");
    hashmap_set(GLOTTA_LANGS, "ts",         "TS");

    hashmap_set(GLOTTA_LANGS, "jsx",        "React JS");
    hashmap_set(GLOTTA_LANGS, "tsx",        "React TS");
    hashmap_set(GLOTTA_LANGS, "vue",        "Vue");
    hashmap_set(GLOTTA_LANGS, "svelte",     "Svelte");
    hashmap_set(GLOTTA_LANGS, "astro",      "Astro");

    hashmap_set(GLOTTA_LANGS, "wasm",       "WebAssembly");

    // Scripting
    hashmap_set(GLOTTA_LANGS, "bat",        "Batchfile");
    hashmap_set(GLOTTA_LANGS, "cmd",        "Batchfile");

    hashmap_set(GLOTTA_LANGS, "ps1",        "PowerShell");
    hashmap_set(GLOTTA_LANGS, "psd1",       "PowerShell");
    hashmap_set(GLOTTA_LANGS, "psm1",       "PowerShell");

    hashmap_set(GLOTTA_LANGS, "sh",         "Shell script");
    hashmap_set(GLOTTA_LANGS, "bash",       "Bash script");
    hashmap_set(GLOTTA_LANGS, "ksh",        "Korn shell script");
    hashmap_set(GLOTTA_LANGS, "zsh",        "Zsh script");
    hashmap_set(GLOTTA_LANGS, "fish",       "Fish script");

    hashmap_set(GLOTTA_LANGS, "awk",        "AWK script");
    hashmap_set(GLOTTA_LANGS, "sed",        "SED script");

    // Data
    hashmap_set(GLOTTA_LANGS, "csv",        "CSV");
    hashmap_set(GLOTTA_LANGS, "xml",        "XML");
    hashmap_set(GLOTTA_LANGS, "json",       "JSON");
    hashmap_set(GLOTTA_LANGS, "txt",        "Plain text");
    hashmap_set(GLOTTA_LANGS, "md",         "Markdown");
    hashmap_set(GLOTTA_LANGS, "markdown",   "Markdown");
    hashmap_set(GLOTTA_LANGS, "toml",       "TOML");
    hashmap_set(GLOTTA_LANGS, "yml",        "YAML");
    hashmap_set(GLOTTA_LANGS, "yaml",       "YAML");
    hashmap_set(GLOTTA_LANGS, "ini",        "INI");
    hashmap_set(GLOTTA_LANGS, "env",        "Env file");
    hashmap_set(GLOTTA_LANGS, "lock",       "Lockfile");
    hashmap_set(GLOTTA_LANGS, "log",        "Log file");
    hashmap_set(GLOTTA_LANGS, "sql",        "SQL");

    return true;
}

#endif // GLOTTA_IMPLEMENTATION
