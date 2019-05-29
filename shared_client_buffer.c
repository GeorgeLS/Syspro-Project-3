#include "shared_client_buffer.h"
#include "common/macros.h"

static size_t __INLINE__
shared_buffer_next_right_index(shared_buffer *buffer) {
    size_t index = (buffer->right + 1) % buffer->table_size;
    return index;
}

static size_t __INLINE__
shared_buffer_next_left_index(shared_buffer *buffer) {
    size_t index = (buffer->left + 1) % buffer->table_size;
    return index;
}

bool client_file_info_contains_file(client_file_info *info) {
    char zero_value[MAX_PATHNAME_SIZE] = {0};
    bool result = memcmp(info->pathname_with_version.pathname, zero_value, MAX_PATHNAME_SIZE) == 0;
    return result;
}

shared_buffer shared_buffer_create(size_t size) {
    shared_buffer buffer = {
            .info_table = __MALLOC__(size, client_file_info),
            .table_size = size,
            .left = 0U,
            .right = 0U
    };
    pthread_mutex_init(&buffer.mutex, NULL);
    return buffer;
}

bool shared_buffer_full(shared_buffer *buffer) {
    bool result = shared_buffer_next_right_index(buffer) == buffer->left;
    return result;
}

bool shared_buffer_emtpy(shared_buffer *buffer) {
    bool result = buffer->left == buffer->right;
    return result;
}

void shared_buffer_push(shared_buffer *buffer, client_file_info *info) {
    pthread_mutex_lock(&buffer->mutex);

    if (shared_buffer_full(buffer)) goto __EXIT__;

    client_file_info *element = &buffer->info_table[buffer->right];
    buffer->info_table[buffer->right] = *info;
    buffer->right = shared_buffer_next_right_index(buffer);

    __EXIT__:
    pthread_mutex_unlock(&buffer->mutex);
}

client_file_info *shared_buffer_pop(shared_buffer *buffer) {
    client_file_info *element = NULL;
    pthread_mutex_lock(&buffer->mutex);

    if (shared_buffer_emtpy(buffer)) goto __EXIT__;

    element = &buffer->info_table[buffer->left];
    buffer->left = shared_buffer_next_left_index(buffer);

    __EXIT__:
    pthread_mutex_unlock(&buffer->mutex);

    return element;
}
