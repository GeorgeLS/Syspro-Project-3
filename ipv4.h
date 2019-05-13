#ifndef EXERCISE_III_IPV4_H
#define EXERCISE_III_IPV4_H

#include <stdbool.h>
#include "common/attributes.h"
#include "common/common_types.h"

#define IPV4_BYTES 4
#define MIN_IVP4_LENGTH 7
#define MAX_IPV4_LENGTH 15
#define MAX_IPV4_OCTET 255
#define MIN_IPV4_OCTET 1

typedef struct ipv4 {
    char *address;
    ubyte address_bytes[IPV4_BYTES];
} ipv4;

__NON_NULL__(1, 2)
bool ipv4_get_address_bytes(const char *address, ubyte *byte_buffer);

__NON_NULL__(1, 2)
void ipv4_initialize(ipv4 *ip, const char *restrict address);

#endif //EXERCISE_III_IPV4_H
