#ifndef EXERCISE_III_SHARED_CLIENT_BUFFER_H
#define EXERCISE_III_SHARED_CLIENT_BUFFER_H

#include <stdatomic.h>
#include <stddef.h>
#include "client_list.h"

#define MAX_PATHNAME_SIZE 128
#define SHA256_SIZE 256

typedef struct client_file_info {
    client_tuple tuple;
    char pathname[MAX_PATHNAME_SIZE];
    char version[SHA256_SIZE];
} client_file_info;

typedef struct shared_buffer {
    client_file_info *info_table;
    pthread_mutex_t mutex;
    size_t table_size;
    size_t left;
    size_t right;
} shared_buffer;

shared_buffer shared_buffer_create(size_t size);

__NON_NULL__(1)
bool shared_buffer_full(shared_buffer *buffer);

__NON_NULL__(1)
bool shared_buffer_emtpy(shared_buffer *buffer);

__NON_NULL__(1, 2)
void shared_buffer_push(shared_buffer *buffer, client_file_info *info);

__NON_NULL__(1) __WARN_UNUSED_RESULT__
client_file_info *shared_buffer_pop(shared_buffer *buffer);


#endif //EXERCISE_III_SHARED_CLIENT_BUFFER_H
