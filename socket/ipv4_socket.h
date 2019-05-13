#ifndef EXERCISE_III_IPV4_SOCKET_H
#define EXERCISE_III_IPV4_SOCKET_H

#include <netinet/in.h>
#include "../common/common_types.h"
#include "../common/attributes.h"

typedef struct ipv4_socket {
    struct sockaddr_in socket;
    int socket_fd;
} ipv4_socket;

__NON_NULL__(2) __WARN_UNUSED_RESULT__
int ipv4_socket_create(u16 port_number, ipv4_socket *out_socket);

__NON_NULL__(1) __WARN_UNUSED_RESULT__
int ipv4_socket_bind(ipv4_socket *socket);

#endif //EXERCISE_III_IPV4_SOCKET_H
