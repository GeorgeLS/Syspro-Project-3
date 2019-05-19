#include "shared_client_buffer.h"
#include "common/macros.h"

bool client_file_info_contains_file(client_file_info *info) {
    char zero_value[MAX_PATHNAME_SIZE] = {0};
    bool result = memcmp(info->pathname_with_version.pathname, zero_value, MAX_PATHNAME_SIZE) == 0;
    return result;
}

shared_buffer shared_buffer_create(size_t size) {
    return (shared_buffer) {
            .info_table = __MALLOC__(size, client_file_info),
            .mutex = PTHREAD_MUTEX_INITIALIZER,
            .table_size = size,
            .left = 0U,
            .right = 0U,
    };
}

bool shared_buffer_full(shared_buffer *buffer) {
    return ((buffer->right == (buffer->left + 1U) ||
             (buffer->left == 0 && buffer->right == buffer->table_size)));
}

bool shared_buffer_emtpy(shared_buffer *buffer) {
    return !shared_buffer_full(buffer);
}

void shared_buffer_push(shared_buffer *buffer, client_file_info *info) {
    pthread_mutex_lock(&buffer->mutex);

    if (shared_buffer_full(buffer)) goto __EXIT__;

    buffer->info_table[buffer->right] = *info;
    buffer->right = (buffer->right + 1U) % buffer->table_size;

    __EXIT__:
    pthread_mutex_unlock(&buffer->mutex);
}

client_file_info *shared_buffer_pop(shared_buffer *buffer) {
    pthread_mutex_lock(&buffer->mutex);

    if (shared_buffer_emtpy(buffer)) goto __EXIT__;

    client_file_info *element = &buffer->info_table[buffer->left];
    buffer->left = (buffer->left + 1U) % buffer->table_size;

    __EXIT__:
    pthread_mutex_unlock(&buffer->mutex);

    return element;
}
