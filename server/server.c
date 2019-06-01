#include <stdbool.h>
#include <stdio.h>
#include "server_utils.h"
#include "../socket/ipv4_socket.h"
#include "../common/report_utils.h"
#include "../common/macros.h"
#include "../common/common_utils.h"

server_options options;
ipv4_socket server_socket;
list connected_clients;

static void setup_server_socket(void) {
    if (ipv4_socket_create(options.port_number, IPV4_ANY_ADDRESS, &server_socket) < 0) {
        die("Couldn't create address for server!");
    }
    if (ipv4_socket_bind(&server_socket) < 0) {
        die("Couldn't bind server address!");
    }
    if (ipv4_socket_listen(&server_socket) < 0) {
        die("Couldn't register server address for listening!");
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) usage();
    options = parse_command_line_arguments(argc, argv);

    connected_clients = list_create(connected_client_equals, !LIST_MULTITHREADED);

    setup_server_socket();

    fd_set sockets_set;
    request_handler_arguments arguments = {
            .connected_clients = &connected_clients,
            .server_socket = &server_socket,
            .set = &sockets_set,
            .directory_name = NULL,
            .shared_buffer = NULL
    };

    while (true) {
        reset_and_add_socket_descriptors_to_set(&sockets_set, server_socket.socket_fd, &connected_clients);
        bool available_for_read = select(FD_SETSIZE, &sockets_set, NULL, NULL, NULL) > 0;
        if (available_for_read) {
            handle_incoming_requests(&arguments);
        }
    }
    return EXIT_SUCCESS;
}