#ifndef EXERCISE_III_MACROS_H
#define EXERCISE_III_MACROS_H

#include <stdlib.h>
#include <string.h>

#define __MALLOC__(size, type) ((type *) malloc((size) * sizeof(type)))

#define IS_EMPTY(structure) ({ \
    typeof(structure) zero_value = {0}; \
    memcmp(&structure, &zero_value, sizeof(structure)) == 0; \
})

#endif //EXERCISE_III_MACROS_H
