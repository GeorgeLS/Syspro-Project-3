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
#include "server_data.h"

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

void handle_log_on_request(ipv4_socket *sender, byte *data, u32 bytes) {
    client_tuple tuple = client_tuple_from_ntoh_bytes(data, bytes);
    connected_client *new_client = connected_client_create(&tuple, sender);

    if (list_element_exists(&connected_clients, &new_client)) {
        return;
    }

    request request = create_user_on_request(new_client->tuple.port_number, new_client->tuple.ip);
    list_node *curr = connected_clients.head;
    if (curr != NULL) {
        do {
            connected_client *connected_client = curr->data;
            if (ipv4_socket_send_request(connected_client->socket, request) < 0) {
                report_error("Couldn't send USER_ON request to existing new_client!");
                goto __NEXT_LOOP__;
            }
            __NEXT_LOOP__:
            curr = curr->next;
        } while (curr != connected_clients.head);
    }
    free_request(&request);
    list_rpush(&connected_clients, new_client);
}

void handle_get_clients_request(connected_client *connected_client) {
    request request = create_client_list_request(&connected_clients, connected_client);
    if (ipv4_socket_send_request(connected_client->socket, request) < 0) {
        report_error("Couldn't send CLIENTS_LIST to client!");
    }
    free_request(&request);
}

void handle_log_off_request(connected_client *client) {
    list_remove(&connected_clients, client);
    request request = create_user_off_request(client->tuple.port_number, client->tuple.ip);

    list_node *curr = connected_clients.head;
    do {
        connected_client *client_to_notify = curr->data;
        if (ipv4_socket_send_request(client_to_notify->socket, request) < 0) {
            report_error("Couldn't send USER_OFF request to existing client!");
            goto __NEXT_LOOP__;
        }
        __NEXT_LOOP__:
        curr = curr->next;
    } while (curr != connected_clients.head);

    free_request(&request);
}


void handle_request(connected_client *connected_client, request request) {
    u32 command_length = request.header.command_length;
    u32 bytes = request.header.bytes;
    byte *data = request.data;
    if (str_n_equals(data, LOG_ON, command_length)) {
        handle_log_on_request(connected_client->socket, data + command_length, bytes - command_length);
    } else if (str_n_equals(data, GET_CLIENTS, command_length)) {
        handle_get_clients_request(connected_client);
    } else if (str_n_equals(data, LOG_OFF, command_length)) {
        handle_log_off_request(connected_client);
    } else {
        report_response("Unknown command! (%.*s)", command_length, data);
    }
}

void serve_client(connected_client *connected_client) {
    request request = ipv4_socket_get_request(connected_client->socket);
    handle_request(connected_client, request);
    free_request(&request);
}

void reset_and_add_socket_descriptors_to_set(fd_set *set) {
    FD_ZERO(set);
    FD_SET(server_socket.socket_fd, set);

    list_node *curr = connected_clients.head;
    if (curr != NULL) {
        do {
            connected_client *connected_client = curr->data;
            FD_SET(connected_client->socket->socket_fd, set);
            curr = curr->next;
        } while (curr != connected_clients.head);
    }
}

void handle_incoming_requests(fd_set *set) {
    if (FD_ISSET(server_socket.socket_fd, set)) {
        ipv4_socket client_socket = {0};
        if (ipv4_socket_accept(&server_socket, &client_socket) < 0) {
            report_error("Couldn't accept an incoming connection!");
        }
        report_response("Accepted connection!");
        connected_client new_client = {
                .socket = &client_socket,
                .tuple = {0}
        };
        serve_client(&new_client);
    }

    list_node *curr = connected_clients.head;
    if (curr != NULL) {
        do {
            connected_client *connected_client = curr->data;
            if (FD_ISSET(connected_client->socket->socket_fd, set)) {
                serve_client(connected_client);
            }
            curr = curr->next;
        } while (curr != connected_clients.head);
    }
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

int main(int argc, char *argv[]) {
    if (argc < 3) usage();
    options = parse_command_line_arguments(argc, argv);

    connected_clients = list_create(connected_client_equals, !LIST_MULTITHREADED);

    setup_server_socket();

    fd_set sockets_set;
    while (true) {
        reset_and_add_socket_descriptors_to_set(&sockets_set);
        bool available_for_read = select(FD_SETSIZE, &sockets_set, NULL, NULL, NULL) > 0;
        if (available_for_read) {
            handle_incoming_requests(&sockets_set);
        }
    }
    return EXIT_SUCCESS;
}

#pragma clang diagnostic pop