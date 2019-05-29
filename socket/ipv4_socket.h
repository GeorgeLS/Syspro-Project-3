#ifndef EXERCISE_III_IPV4_SOCKET_H
#define EXERCISE_III_IPV4_SOCKET_H

#include <netinet/in.h>
#include <netdb.h>
#include "../common/common_types.h"
#include "../common/attributes.h"
#include "../requests.h"

#define IPV4_SOCKET_BACKLOG_SIZE 5
#define IPV4_ANY_ADDRESS (struct in_addr) {htonl(INADDR_ANY)}
#define IPV4_ADDRESS_SIZE (sizeof(u32) + sizeof(u16))
#define IPV4_BYTES 4
#define MIN_IVP4_LENGTH 7
#define MAX_IPV4_LENGTH 15
#define MAX_IPV4_OCTET 255
#define MIN_IPV4_OCTET 1

typedef struct ipv4_socket {
    struct sockaddr_in address;
    int socket_fd;
} ipv4_socket;

__NON_NULL__(3) __WARN_UNUSED_RESULT__
int ipv4_socket_create(u16 port_number, struct in_addr in_address, ipv4_socket *out_socket);

__NON_NULL__(1) __WARN_UNUSED_RESULT__
int ipv4_socket_bind(ipv4_socket *socket);

__NON_NULL__(1) __WARN_UNUSED_RESULT__
int ipv4_socket_listen(ipv4_socket *socket);

__NON_NULL__(1) __WARN_UNUSED_RESULT__
int ipv4_socket_accept(ipv4_socket *server_socket, ipv4_socket *client_socket);

__NON_NULL__(1) __WARN_UNUSED_RESULT__
int ipv4_socket_connect(ipv4_socket *socket);

__NON_NULL__(1, 2) __WARN_UNUSED_RESULT__
bool ipv4_socket_create_and_connect(client_tuple *tuple, ipv4_socket *socket_out);

__NON_NULL__(1) __WARN_UNUSED_RESULT__
ssize_t ipv4_socket_send_request(ipv4_socket *receiver, request request);

__NON_NULL__(1)
request ipv4_socket_get_request(struct ipv4_socket *sender);

#endif //EXERCISE_III_IPV4_SOCKET_H
