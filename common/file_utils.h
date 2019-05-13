#ifndef EXERCISE_III_FILE_UTILS_H
#define EXERCISE_III_FILE_UTILS_H

#include <stddef.h>
#include "attributes.h"

typedef struct entire_file {
    void *data;
    size_t size;
} entire_file;

__NON_NULL__(1)
entire_file read_entire_file_into_memory(const char *filename);

__NON_NULL__(1)
int read_line_from_stdin(void *restrict stdin_buffer, size_t bytes);

#endif //EXERCISE_III_FILE_UTILS_H