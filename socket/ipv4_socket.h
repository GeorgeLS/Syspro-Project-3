#ifndef EXERCISE_III_IPV4_SOCKET_H
#define EXERCISE_III_IPV4_SOCKET_H

#include <netinet/in.h>
#include <netdb.h>
#include "../common/common_types.h"
#include "../common/attributes.h"
#include "../commands.h"

#define IPV4_SOCKET_BACKLOG_SIZE 5
#define IPV4_ANY_ADDRESS (struct in_addr) {htonl(INADDR_ANY)}

typedef struct ipv4_socket {
    struct sockaddr_in address;
    int socket_fd;
} ipv4_socket;

__NON_NULL__(3) __WARN_UNUSED_RESULT__
int ipv4_socket_create(u16 port_number, struct in_addr in_address, ipv4_socket *out_socket);

__NON_NULL__(1, 3)
void ipv4_socket_initialize_from_address(ipv4_socket *socket, u16 port_number, const char *address);

__NON_NULL__(1) __WARN_UNUSED_RESULT__
int ipv4_socket_bind(ipv4_socket *socket);

__NON_NULL__(1) __WARN_UNUSED_RESULT__
int ipv4_socket_listen(ipv4_socket *socket);

__NON_NULL__(1) __WARN_UNUSED_RESULT__
int ipv4_socket_accept(ipv4_socket *server_socket, ipv4_socket *client_socket);

__NON_NULL__(1) __WARN_UNUSED_RESULT__
int ipv4_socket_connect(ipv4_socket *socket);

__NON_NULL__(1) __WARN_UNUSED_RESULT__
ssize_t ipv4_socket_send_request(ipv4_socket *receiver, request request);

__NON_NULL__(1)
request get_request(struct ipv4_socket *sender);

#endif //EXERCISE_III_IPV4_SOCKET_H
