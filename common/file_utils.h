#ifndef EXERCISE_III_FILE_UTILS_H
#define EXERCISE_III_FILE_UTILS_H

#include <stddef.h>
#include <stdbool.h>
#include "attributes.h"
#include "../shared_client_buffer.h"

typedef struct entire_file {
    void *data;
    size_t size;
} entire_file;

__NON_NULL__(1)
entire_file read_entire_file_into_memory(const char *filename);

__NON_NULL__(1)
int read_line_from_stdin(void *restrict stdin_buffer, size_t bytes);

__NON_NULL__(1, 2, 3)
bool get_all_pathnames_and_versions(const char *restrict root_directory,
                                    versioned_pathname **pathnames_out,
                                    size_t *pathnames_n_out);

#endif //EXERCISE_III_FILE_UTILS_H
