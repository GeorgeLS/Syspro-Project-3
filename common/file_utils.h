#ifndef EXERCISE_III_FILE_UTILS_H
#define EXERCISE_III_FILE_UTILS_H

#include <ustat.h>
#include <stddef.h>
#include <stdbool.h>
#include "attributes.h"
#include "../shared_client_buffer.h"
#include "../generic_multithreaded_list.h"

typedef struct entire_file {
    void *data;
    size_t size;
} entire_file;

u64 compute_file_hash(entire_file file);

__NON_NULL__(1)
entire_file read_entire_file_into_memory(const char *filename);

__NON_NULL__(1)
int read_line_from_stdin(void *restrict stdin_buffer, size_t bytes);

__NON_NULL__(1, 2)
void get_all_pathnames_and_versions(const char *restrict root_directory, list *pathnames_out);

__NON_NULL__(1)
bool file_exists(const char *restrict path);

__NON_NULL__(1)
bool directory_exists(const char *restrict path);

__NON_NULL__(1)
bool create_directory(char *restrict path, mode_t permissions);

#endif //EXERCISE_III_FILE_UTILS_H