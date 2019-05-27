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
#include "../client/client_data.h"
#include "../ipv4.h"

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

void handle_log_on_request(byte *data, u32 bytes) {
    client_tuple tuple = client_tuple_from_ntoh_bytes(data, bytes);
    if (list_element_exists(&connected_clients, &tuple)) {
        return;
    }
    request request = create_user_on_request(tuple.port_number, tuple.ip);
    list_node *curr = connected_clients.head;
    do {
        ipv4_socket client_socket;
        if (!ipv4_socket_create_and_connect(&tuple, &client_socket)) {
            goto __NEXT_LOOP__;
        }
        if (ipv4_socket_send_request(&client_socket, request) < 0) {
            report_error("Couldn't send USER_ON request to existing client!");
            goto __NEXT_LOOP__;
        }
        __NEXT_LOOP__:
        curr = curr->next;
    } while (curr != connected_clients.head);
    free_request(&request);
    list_rpush(&connected_clients, &tuple);
}

void handle_get_clients_request(ipv4_socket *sender) {
    char ip[MAX_IPV4_LENGTH];
    inet_ntop(AF_INET, &sender->address.sin_addr, ip, MAX_IPV4_LENGTH);
    client_tuple tuple = {
            .ip = ip,
            .port_number = ntohs(sender->address.sin_port)
    };
    request request = create_client_list_request(&connected_clients, &tuple);
    if (ipv4_socket_send_request(sender, request) < 0) {
        report_error("Couldn't send CLIENTS_LIST to client!");
    }
    free_request(&request);
}

void handle_log_off_request(ipv4_socket *sender) {

}

void handle_request(ipv4_socket *sender, request request) {
    u32 command_length = request.header.command_length;
    u32 bytes = request.header.bytes;
    byte *data = request.data;
    if (str_n_equals(data, LOG_ON, command_length)) {
        handle_log_on_request(data + command_length, bytes - command_length);
    } else if (str_n_equals(data, GET_CLIENTS, command_length)) {
        handle_get_clients_request(sender);
    } else if (str_n_equals(data, LOG_OFF, command_length)) {
        handle_log_off_request(sender);
    } else {
        report_response("Unknown command! (%.*s)", command_length, data);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) usage();
    options = parse_command_line_arguments(argc, argv);

    connected_clients = list_create(client_tuple_equals, !LIST_MULTITHREADED);

    setup_server_socket();

    while (true) {
        ipv4_socket client_socket = {0};
        if (ipv4_socket_accept(&server_socket, &client_socket) < 0) {
            report_error("Couldn't accept an incoming connection!");
            continue;
        }
        report_response("Accepted connection!");
        request request = ipv4_socket_get_request(&client_socket);
        handle_request(&client_socket, request);
        free_request(&request);
        break;
    }
    return EXIT_SUCCESS;
}