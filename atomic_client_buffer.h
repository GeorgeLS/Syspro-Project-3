#ifndef EXERCISE_III_ATOMIC_CLIENT_BUFFER_H
#define EXERCISE_III_ATOMIC_CLIENT_BUFFER_H

#include <stdatomic.h>
#include <stddef.h>

struct client_file_info;

typedef struct atomic_buffer {
    struct client_file_info *info_table;
    size_t table_size;
    atomic_size_t left;
    atomic_size_t right;
    atomic_size_t max_left;
    atomic_size_t max_right;
} atomic_buffer;

#endif //EXERCISE_III_ATOMIC_CLIENT_BUFFER_H
