#ifndef EXERCISE_III_STRING_UTILS_H
#define EXERCISE_III_STRING_UTILS_H

#include <stdbool.h>
#include <stddef.h>
#include "attributes.h"
#include "common_types.h"

__NON_NULL__(1, 2)
bool str_to_ui64_with_end_ptr(const char *source, u64 *destination, char **end_ptr);

__NON_NULL__(1, 2)
bool str_to_ui64(const char *source, u64 *destination);

__NON_NULL__(1)
size_t str_count_occurrences(const char *source, char value);

#endif //EXERCISE_III_STRING_UTILS_H
