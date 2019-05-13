#include "ipv4_socket.h"
#include <strings.h>

int ipv4_socket_create(u16 port_number, ipv4_socket *out_socket) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return -1;
    struct sockaddr_in new_socket;
    bzero(&new_socket, sizeof(struct sockaddr_in));
    new_socket.sin_family = AF_INET;
    new_socket.sin_addr.s_addr = INADDR_ANY;
    new_socket.sin_port = htons(port_number);
    out_socket->socket = new_socket;
    out_socket->socket_fd = fd;
    return 0;
}

int ipv4_socket_bind(ipv4_socket *socket) {
    return bind(socket->socket_fd, (struct sockaddr *) &socket->socket, sizeof(socket->socket));
}
