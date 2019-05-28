#ifndef EXERCISE_III_SERVER_DATA_H
#define EXERCISE_III_SERVER_DATA_H

#include "../client/client_data.h"
#include "../socket/ipv4_socket.h"

typedef struct connected_client {
    client_tuple tuple;
    ipv4_socket *socket;
} connected_client;

__NON_NULL__(1, 2) __WARN_UNUSED_RESULT__
connected_client *connected_client_create(client_tuple *tuple, ipv4_socket *socket);

__NON_NULL__(1, 2)
int connected_client_equals(void *v1, void *v2);

#endif //EXERCISE_III_SERVER_DATA_H
