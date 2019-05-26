#include "ipv4_socket.h"
#include "../common/macros.h"
#include <strings.h>
#include <stddef.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdio.h>
#include <zconf.h>
#include <sys/ioctl.h>

int ipv4_socket_create(u16 port_number, struct in_addr in_address, ipv4_socket *out_socket) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return -1;
//    ioctl(fd, FIONBIO, &(int) {0});
    struct sockaddr_in new_socket;
    bzero(&new_socket, sizeof(struct sockaddr_in));
    new_socket.sin_family = AF_INET;
    new_socket.sin_addr.s_addr = in_address.s_addr;
    new_socket.sin_port = htons(port_number);
    out_socket->address = new_socket;
    out_socket->socket_fd = fd;
    return 0;
}

void ipv4_socket_initialize_from_address(ipv4_socket *socket, u16 port_number, const char *address) {
    struct sockaddr_in *socket_address = &socket->address;
    socket_address->sin_port = htons(port_number);
    socket_address->sin_family = AF_INET;
    socket_address->sin_addr.s_addr = inet_addr(address);
    socket->socket_fd = -1;
}

int ipv4_socket_bind(ipv4_socket *socket) {
    return bind(socket->socket_fd, (struct sockaddr *) &socket->address, sizeof(socket->address));
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
    socklen_t client_length = sizeof(client_socket->address);
    int socket_fd = accept(server_socket->socket_fd,
                           (struct sockaddr *) &client_socket->address,
                           &client_length);
    client_socket->socket_fd = socket_fd;
    return socket_fd;
}

int ipv4_socket_connect(ipv4_socket *socket) {
    return connect(socket->socket_fd,
                   (const struct sockaddr *) &socket->address,
                   sizeof(socket->address));
}

ssize_t ipv4_socket_send_request(ipv4_socket *receiver, request request) {
    u32 bytes = request.header.bytes;
    request.header = header_hton(request.header);
    if (write(receiver->socket_fd, &request.header, sizeof(request_header)) < 0) {
        return -1;
    }
    if (write(receiver->socket_fd, request.data, bytes) < 0) {
        return -1;
    }
    return 0;
}

request ipv4_socket_get_request(ipv4_socket *sender) {
    request_header header = {0};
    if (read(sender->socket_fd, &header, sizeof(request_header)) <= 0) {
        return (request) {0};
    }
    header = header_ntoh(header);

    byte *data = __MALLOC__(header.bytes, byte);

    if (read(sender->socket_fd, data, header.bytes) <= 0) {
        free(data);
        return (request) {0};
    }

    request result = {
            .data = data,
            .header = header
    };
    return result;
}