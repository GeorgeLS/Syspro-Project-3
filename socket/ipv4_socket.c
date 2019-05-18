#include "ipv4_socket.h"
#include "../common/macros.h"
#include <strings.h>
#include <stddef.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdio.h>
#include <zconf.h>

int ipv4_socket_create(u16 port_number, struct in_addr in_address, ipv4_socket *out_socket) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return -1;
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
    int socket_fd = accept(server_socket->socket_fd, (struct sockaddr *) &client_socket->address, &client_length);
    client_socket->socket_fd = socket_fd;
    return socket_fd;
}

int ipv4_socket_connect(ipv4_socket *socket) {
    return connect(socket->socket_fd, (const struct sockaddr *) &socket->address,
                   sizeof(socket->address));
}

ssize_t ipv4_socket_send_request(ipv4_socket *receiver, request request) {
    const struct sockaddr *receiver_address = (const struct sockaddr *) &receiver->address;
    const size_t receiver_length = sizeof(receiver->address);

    if (sendto(receiver->socket_fd,
               &request.header,
               sizeof(request_header),
               0,
               receiver_address,
               receiver_length) < 0) {
        return -1;
    }

    return sendto(receiver->socket_fd,
                  request.data,
                  request.header.bytes,
                  0,
                  receiver_address,
                  receiver_length);
}

request get_request(struct ipv4_socket *sender) {
    request_header header;
    struct sockaddr *sender_address = (struct sockaddr *) &sender->address;
    socklen_t sender_length = sizeof(sender->address);

    if (recvfrom(sender->socket_fd,
                 &header,
                 sizeof(request_header),
                 0,
                 sender_address,
                 &sender_length) < 0) {
        return (request) {0};
    }

    header = header_ntoh(header);

    if (header.bytes == 0U) return (request) {0};

    request new_request = {
            .data = __MALLOC__(header.bytes, byte),
            .header = header
    };

    if (recvfrom(sender->socket_fd,
                 new_request.data,
                 new_request.header.bytes,
                 0,
                 sender_address,
                 &sender_length) < 0) {

        return (request) {0};
    }

    return new_request;
}