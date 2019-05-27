#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
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

client_tuple client_tuple_from_ntoh_bytes(byte *restrict data, u32 bytes) {
    client_tuple tuple;
    u32 binary_ip = *(u32 *) data;
    char ip[MAX_IPV4_LENGTH] = {0};
    inet_ntop(AF_INET, &binary_ip, ip, MAX_IPV4_LENGTH);
    u16 port_number = ntohs(*((u16 *) (data + sizeof(u32))));
    tuple.ip = strdup(ip);
    tuple.port_number = port_number;
    return tuple;
}