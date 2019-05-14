#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <zconf.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "server_utils.h"
#include "../socket/ipv4_socket.h"
#include "../common/report_utils.h"

int main(int argc, char *argv[]) {
    if (argc < 3) usage();
    server_options options = parse_command_line_arguments(argc, argv);
    ipv4_socket socket;
    if (ipv4_socket_create(options.port_number, IPV4_ANY_ADDRESS, &socket) < 0) {
        die("Couldn't create socket for server!");
    }
    if (ipv4_socket_bind(&socket) < 0) {
        die("Couldn't bind server socket!");
    }
    if (ipv4_socket_listen(&socket) < 0) {
        die("Couldn't register server socket for listening!");
    }
    ipv4_socket client_socket = {0};
    int client_socket_fd;
    if ((client_socket_fd = ipv4_socket_accept(&socket, &client_socket)) < 0) {
        die("There was an error while accepting client connection");
    }
    report_response("Accepted connection!");
    char buffer[512] = {0};
    while (read(client_socket_fd, buffer, sizeof(buffer)) > 0) {
        for (size_t i = 0U; buffer[i] != '\0'; ++i) {
            putchar(toupper(buffer[i]));
        }
    }
    putchar('\n');
    write(client_socket_fd, buffer, sizeof(buffer));
    report_response("Closing connection [SERVER]");
    close(client_socket_fd);
    return EXIT_SUCCESS;
}