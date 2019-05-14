#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <inttypes.h>
#include <zconf.h>
#include <string.h>
#include "client_utils.h"
#include "../socket/ipv4_socket.h"
#include "../common/report_utils.h"

client_options options;
ipv4_socket client_socket;

void setup_client_socket() {
    if (ipv4_socket_create(options.port_number, IPV4_ANY_ADDRESS, &client_socket) < 0) {
        die("Could not create client socket!");
    }
    if (ipv4_socket_bind(&client_socket) < 0) {
        die("Could not bind client socket!");
    }
}

void connect_to_server(ipv4_socket *socket) {
    ipv4_socket_initialize_from_address(socket, options.server_port_number, options.server_ip.address);
    if (ipv4_socket_connect(socket, &client_socket) < 0) {
        die("Couldn't connect to the server!");
    }
    report_response("Connected to server %s with port %" PRIu16, options.server_ip.address, options.server_port_number);
}

int main(int argc, char *argv[]) {
    if (argc < 13) usage();
    options = parse_command_line_arguments(argc, argv);
    setup_client_socket();
    ipv4_socket server_socket;
    connect_to_server(&server_socket);
    char buffer[512];
    printf("Give input: ");
    fgets(buffer, sizeof(buffer), stdin);
    write(client_socket.socket_fd, buffer, sizeof(buffer));
    read(client_socket.socket_fd, buffer, sizeof(buffer));
    report_response("Received from server: %s", buffer);
    close(client_socket.socket_fd);
    return EXIT_SUCCESS;
}