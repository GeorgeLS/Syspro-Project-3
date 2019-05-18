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
#include "../common/macros.h"
#include "../common/string_utils.h"

server_options options;
ipv4_socket server_socket;
client_list connected_clients;

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

    connected_clients = client_list_create(!CLIENT_LIST_MULTITHREADED);
    client_tuple tuple = {
            .ip = "192.168.0.1",
            .port_number = 2500
    };
    client_list_rpush(&connected_clients, &tuple);

    tuple.ip = "182.123.40.12";
    tuple.port_number = 6000;
    client_list_rpush(&connected_clients, &tuple);

    setup_server_socket();

    ipv4_socket client_socket = {0};
    int client_socket_fd;
    if ((client_socket_fd = ipv4_socket_accept(&server_socket, &client_socket)) < 0) {
        die("There was an error while accepting client connection");
    }
    report_response("Accepted connection!");

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

    while (true) {
        request new_request = get_request(&client_socket);
        if (new_request.data == NULL) continue;
        if (str_n_equals(new_request.data, GET_CLIENTS, new_request.header.command_length)) {
            request request = create_client_list_request(&connected_clients);
            if (ipv4_socket_send_request(&client_socket, request) < 0) {
                report_error("Couldn't send client list to client");
            }
            free_request(&request);
        } else {
            report_response("Received from client: %.*s", new_request.header.command_length, new_request.data);
        }
        free_request(&new_request);
    }

#pragma clang diagnostic pop
    report_response("Closing connection [SERVER]");
    close(client_socket_fd);
    return EXIT_SUCCESS;
}