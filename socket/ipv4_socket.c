#include "ipv4_socket.h"
#include <strings.h>
#include <stddef.h>
#include <string.h>
#include <arpa/inet.h>

int ipv4_socket_create(u16 port_number, struct in_addr in_address, ipv4_socket *out_socket) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return -1;
    struct sockaddr_in new_socket;
    bzero(&new_socket, sizeof(struct sockaddr_in));
    new_socket.sin_family = AF_INET;
    new_socket.sin_addr.s_addr = in_address.s_addr;
    new_socket.sin_port = htons(port_number);
    out_socket->socket = new_socket;
    out_socket->socket_fd = fd;
    return 0;
}

void ipv4_socket_initialize_from_address(ipv4_socket *socket, u16 port_number, const char *address) {
    struct sockaddr_in *socket_address = &socket->socket;
    socket_address->sin_port = htons(port_number);
    socket_address->sin_family = AF_INET;
    socket_address->sin_addr.s_addr = inet_addr(address);
    socket->socket_fd = -1;
}

int ipv4_socket_bind(ipv4_socket *socket) {
    return bind(socket->socket_fd, (struct sockaddr *) &socket->socket, sizeof(socket->socket));
}

int ipv4_socket_listen(ipv4_socket *socket) {
    return listen(socket->socket_fd, IPV4_SOCKET_BACKLOG_SIZE);
}

int ipv4_socket_accept(ipv4_socket *server_socket, ipv4_socket *client_socket) {
    // We do this in order to simulate the case where we would just pass NULL to accept system call
    ipv4_socket placeholder = {0};
    if (client_socket == NULL) {
        client_socket = &placeholder;
    }
    socklen_t client_length = sizeof(client_socket->socket);
    return accept(server_socket->socket_fd, (struct sockaddr *) &client_socket->socket, &client_length);
}

int ipv4_socket_connect(ipv4_socket *server_socket, ipv4_socket *client_socket) {
    return connect(client_socket->socket_fd, (const struct sockaddr *) &server_socket->socket,
                   sizeof(client_socket->socket));
}
