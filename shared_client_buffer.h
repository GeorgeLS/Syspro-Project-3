#ifndef EXERCISE_III_SHARED_CLIENT_BUFFER_H
#define EXERCISE_III_SHARED_CLIENT_BUFFER_H

#include <stdatomic.h>
#include <stddef.h>
#include <pthread.h>
#include "common/common_types.h"
#include "client/client_data.h"

#define MAX_PATHNAME_SIZE 128

typedef struct versioned_pathname {
    char pathname[MAX_PATHNAME_SIZE];
    u64 version;
} versioned_pathname;

typedef struct client_file_info {
    client_tuple tuple;
    versioned_pathname pathname_with_version;
} client_file_info;

typedef struct shared_buffer {
    client_file_info *info_table;
    pthread_cond_t condition_empty;
    pthread_cond_t condition_full;
    pthread_mutex_t mutex;
    size_t table_size;
    size_t left;
    size_t right;
} shared_buffer;

__NON_NULL__(1)
bool client_file_info_contains_file(client_file_info *info);

shared_buffer shared_buffer_create(size_t size);

void shared_bufffer_destroy(shared_buffer *buffer);

__NON_NULL__(1)
bool shared_buffer_full(shared_buffer *buffer);

__NON_NULL__(1)
bool shared_buffer_emtpy(shared_buffer *buffer);

__NON_NULL__(1, 2)
void shared_buffer_push(shared_buffer *buffer, client_file_info *info);

__NON_NULL__(1) __WARN_UNUSED_RESULT__
client_file_info *shared_buffer_pop(shared_buffer *buffer);

#endif //EXERCISE_III_SHARED_CLIENT_BUFFER_H
