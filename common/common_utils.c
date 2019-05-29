#include <zconf.h>
#include "common_utils.h"
#include "../generic_multithreaded_list.h"
#include "common_data.h"
#include "report_utils.h"
#include "string_utils.h"

void reset_and_add_socket_descriptors_to_set(fd_set *set, int server_fd, list *_list) {
    FD_ZERO(set);
    FD_SET(server_fd, set);

    list_node *curr = _list->head;
    if (curr != NULL) {
        do {
            connected_client *connected_client = curr->data;
            FD_SET(connected_client->socket.socket_fd, set);
            curr = curr->next;
        } while (curr != _list->head);
    }
}

void handle_log_on_request(ipv4_socket *sender, byte *data, list *connected_clients) {
    client_tuple tuple = client_tuple_from_ntoh_bytes(data);
    connected_client *new_client = connected_client_create(&tuple, sender);

    if (list_element_exists(connected_clients, &new_client)) {
        return;
    }

    request request = create_user_on_request(new_client->tuple.port_number, new_client->tuple.ip);
    list_node *curr = connected_clients->head;
    if (curr != NULL) {
        do {
            connected_client *connected_client = curr->data;
            if (ipv4_socket_send_request(&connected_client->socket, request) < 0) {
                report_error("Couldn't send USER_ON request to existing new_client!");
                goto __NEXT_LOOP__;
            }
            __NEXT_LOOP__:
            curr = curr->next;
        } while (curr != connected_clients->head);
    }
    free_request(&request);
    list_rpush(connected_clients, new_client);
}

void handle_get_clients_request(connected_client *connected_client, list *connected_clients) {
    request request = create_client_list_request(connected_clients, connected_client);
    if (ipv4_socket_send_request(&connected_client->socket, request) < 0) {
        report_error("Couldn't send CLIENTS_LIST to client!");
    }
    free_request(&request);
}

void handle_log_off_request(connected_client *client, list *connected_clients) {
    list_remove(connected_clients, client);
    request request = create_user_off_request(client->tuple.port_number, client->tuple.ip);

    list_node *curr = connected_clients->head;
    do {
        connected_client *client_to_notify = curr->data;
        if (ipv4_socket_send_request(&client_to_notify->socket, request) < 0) {
            report_error("Couldn't send USER_OFF request to existing client!");
            goto __NEXT_LOOP__;
        }
        __NEXT_LOOP__:
        curr = curr->next;
    } while (curr != connected_clients->head);

    free_request(&request);
}

void handle_request(list *connected_clients, connected_client *connected_client, request request) {
    u32 command_length = request.header.command_length;
    byte *data = request.data;
    if (str_n_equals(data, LOG_ON, command_length)) {
        handle_log_on_request(&connected_client->socket, data + command_length, connected_clients);
    } else if (str_n_equals(data, GET_CLIENTS, command_length)) {
        handle_get_clients_request(connected_client, connected_clients);
    } else if (str_n_equals(data, LOG_OFF, command_length)) {
        handle_log_off_request(connected_client, connected_clients);
    } else {
        report_response("Unknown command! (%.*s)", command_length, data);
    }
}

void serve_client(list *connected_clients, connected_client *connected_client) {
    request request = ipv4_socket_get_request(&connected_client->socket);
    handle_request(connected_clients, connected_client, request);
    free_request(&request);
}

void handle_incoming_requests(fd_set *set, ipv4_socket *server_socket, list *connected_clients) {
    if (FD_ISSET(server_socket->socket_fd, set)) {
        ipv4_socket client_socket = {0};
        if (ipv4_socket_accept(server_socket, &client_socket) < 0) {
            report_error("Couldn't accept an incoming connection!");
        }
        report_response("Accepted connection!");
        connected_client new_client = {
                .socket = client_socket,
                .tuple = {0}
        };
        serve_client(connected_clients, &new_client);
    }

    list_node *curr = connected_clients->head;
    if (curr != NULL) {
        do {
            connected_client *connected_client = curr->data;
            if (FD_ISSET(connected_client->socket.socket_fd, set)) {
                serve_client(connected_clients, connected_client);
            }
            curr = curr->next;
        } while (curr != connected_clients->head);
    }
}