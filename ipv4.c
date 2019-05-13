#include "ipv4.h"
#include "common/string_utils.h"
#include "common/common_types.h"
#include <string.h>

bool ipv4_get_address_bytes(const char *address, ubyte *byte_buffer) {
    size_t address_length = strlen(address);
    if (address_length < MIN_IVP4_LENGTH || address_length > MAX_IPV4_LENGTH) {
        return false;
    }
    size_t dots = str_count_occurrences(address, '.');
    if (dots != 3) return false;
    char *current_dot;
    size_t index = 0U;
    u64 byte_value;
    do {
        if (!str_to_ui64_with_end_ptr(address, &byte_value, &current_dot)) {
            return false;
        }
        byte_buffer[index++] = byte_value;
        address = ++current_dot;
    } while (*address != '\0');
    return true;
}

void ipv4_initialize(ipv4 *ip, const char *restrict address) {
    ip->address = strdup(address);
    ipv4_get_address_bytes(address, ip->address_bytes);
}