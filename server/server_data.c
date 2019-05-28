#include "server_data.h"
#include "../common/macros.h"

int connected_client_equals(void *v1, void *v2) {
    connected_client *c1 = v1;
    connected_client *c2 = v2;
    int result = client_tuple_equals(&c1->tuple, &c2->tuple);
    return result;
}

connected_client *connected_client_create(client_tuple *tuple, ipv4_socket *socket) {
    connected_client *new_client = __MALLOC__(1, connected_client);
    new_client->socket = socket;
    new_client->tuple = *tuple;
    return new_client;
}
