#include "string_utils.h"
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <string.h>

bool str_to_ui64_with_end_ptr(const char *source, uint64_t *destination, char **end_ptr) {
    errno = 0;
    uint64_t value = strtol(source, end_ptr, 10);
    if ((errno == ERANGE && value == LONG_MAX)
        || (errno != 0 && value == 0U)
        || (end_ptr != NULL && *end_ptr == source)) {
        return false;
    }
    *destination = value;
    return true;
}

bool str_to_ui64(const char *source, uint64_t *destination) {
    return str_to_ui64_with_end_ptr(source, destination, NULL);
}

size_t str_count_occurrences(const char *source, char value) {
    size_t occurrences = 0U;
    size_t source_length = strlen(source);
    for (size_t i = 0U; i != source_length; ++i) {
        if (source[i] == value) ++occurrences;
    }
    return occurrences;
}