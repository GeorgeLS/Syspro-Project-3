#ifndef EXERCISE_III_CLIENT_DATA_H
#define EXERCISE_III_CLIENT_DATA_H

#include <stdbool.h>
#include "../common/attributes.h"
#include "../common/common_types.h"

typedef struct client_tuple {
    char *ip;
    u16 port_number;
} client_tuple;

__NON_NULL__(1, 2)
int client_tuple_equals(void *tuple1, void *tuple2);

__NON_NULL__(1)
client_tuple client_tuple_from_ntoh_bytes(byte *restrict data);

#endif //EXERCISE_III_CLIENT_DATA_H
