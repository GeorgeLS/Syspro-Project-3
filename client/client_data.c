#include "client_data.h"
#include "../common/string_utils.h"
#include "../ipv4.h"

int client_tuple_equals(void *tuple1, void *tuple2) {
    client_tuple *t1 = tuple1;
    client_tuple *t2 = tuple2;
    int result = (str_n_equals(t1->ip, t2->ip, MAX_IPV4_LENGTH) &&
                  t1->port_number == t2->port_number) ? 1 : 0;
    return result;
}