#include <zconf.h>
#include <stdio.h>
#include <arpa/inet.h>
#include "common_utils.h"
#include "../generic_multithreaded_list.h"
#include "common_data.h"
#include "report_utils.h"
#include "string_utils.h"
#include "file_utils.h"
#include "macros.h"

void reset_and_add_socket_descriptors_to_set(fd_set *set, int server_fd, list *connected_clients) {
    FD_ZERO(set);
    FD_SET(server_fd, set);

    list_node *curr = connected_clients->head;
    if (curr != NULL) {
        do {
            connected_client *connected_client = curr->data;
            if (connected_client->socket.socket_fd != -1) {
                FD_SET(connected_client->socket.socket_fd, set);
            }
            curr = curr->next;
        } while (curr != connected_clients->head);
    }
}

static void
handle_log_on_request(ipv4_socket *sender, byte *data, list *connected_clients) {
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
            if (!ipv4_socket_create_and_connect(&connected_client->tuple, &connected_client->socket)) {
                report_error("Couldn't connect to client");
                goto __NEXT_LOOP__;
            }
            if (ipv4_socket_send_request(&connected_client->socket, request) < 0) {
                report_error("Couldn't send USER_ON request to existing new_client!");
                goto __NEXT_LOOP__;
            }
            close(connected_client->socket.socket_fd);
            connected_client->socket.socket_fd = -1;

            __NEXT_LOOP__:
            curr = curr->next;
        } while (curr != connected_clients->head);
    }
    free_request(&request);
    list_rpush(connected_clients, new_client);
}

static void
handle_get_clients_request(connected_client *connected_client, list *connected_clients) {
    request request = create_client_list_request(connected_clients, connected_client);
    if (ipv4_socket_send_request(&connected_client->socket, request) < 0) {
        report_error("Couldn't send CLIENTS_LIST to client!");
    }
    free_request(&request);
    close(connected_client->socket.socket_fd);
    connected_client->socket.socket_fd = -1;
}

static void
handle_log_off_request(connected_client *client, list *connected_clients) {
    close(client->socket.socket_fd);
    list_remove(connected_clients, client);
    request request = create_user_off_request(client->tuple.port_number, client->tuple.ip);

    list_node *curr = connected_clients->head;
    do {
        connected_client *client_to_notify = curr->data;
        if (!ipv4_socket_create_and_connect(&client_to_notify->tuple, &client_to_notify->socket)) {
            report_error("Couldn't connect to client!");
            goto __NEXT_LOOP__;
        }
        if (ipv4_socket_send_request(&client_to_notify->socket, request) < 0) {
            report_error("Couldn't send USER_OFF request to existing client!");
            goto __NEXT_LOOP__;
        }
        close(client_to_notify->socket.socket_fd);
        client_to_notify->socket.socket_fd = -1;

        __NEXT_LOOP__:
        curr = curr->next;
    } while (curr != connected_clients->head);

    free_request(&request);
}

static void
handle_user_on_request(connected_client *sender, byte *data, list *connected_clients, shared_buffer *buffer) {
    client_tuple tuple = client_tuple_from_ntoh_bytes(data);
    connected_client *new_client = connected_client_create(&tuple, &(ipv4_socket) {.socket_fd = -1});

    if (list_element_exists(connected_clients, &new_client)) {
        return;
    }

    list_rpush(connected_clients, new_client);
    client_file_info info = {
            .tuple = tuple,
            .pathname_with_version = {0}
    };
    close(sender->socket.socket_fd);
    sender->socket.socket_fd = -1;
    shared_buffer_push(buffer, &info);
}

static void
handle_user_off_request(connected_client *sender, list *connected_clients, byte *data) {
    client_tuple tuple = client_tuple_from_ntoh_bytes(data);
    connected_client key = {
            .tuple = tuple
    };
    connected_client *client_left = list_find_element(connected_clients, &key);
    list_remove(connected_clients, client_left);
    close(sender->socket.socket_fd);
    sender->socket.socket_fd = -1;
}

static void
handle_get_file_list_request(connected_client *connected_client, char *restrict directory_name) {
    request request = create_file_list_request(directory_name);
    if (ipv4_socket_send_request(&connected_client->socket, request) < 0) {
        report_error("Couldn't send FILE_LIST request to client");
    }
    free_request(&request);
    close(connected_client->socket.socket_fd);
    connected_client->socket.socket_fd = -1;
}

static void
handle_get_file_request(connected_client *connected_client, byte *data, u32 bytes) {
    byte *address_of_version = data + bytes - sizeof(u64);
    u64 version = *(u64 *) address_of_version;
    address_of_version[0] = '\0'; // Null terminate so we can keep the pathname

    if (!file_exists(data)) {
        request file_not_found_request = create_file_not_found_request();
        if (ipv4_socket_send_request(&connected_client->socket, file_not_found_request) < 0) {
            report_error("Couldn't send FILE_NOT_FOUND to client!");
        }
        free_request(&file_not_found_request);
    } else {
        entire_file file = read_entire_file_into_memory(data);
        u64 current_version = compute_file_hash(file);
        if (current_version == version) {
            request file_up_to_date_request = create_file_up_to_date_request();
            if (ipv4_socket_send_request(&connected_client->socket, file_up_to_date_request) < 0) {
                report_error("Couldn't send FILE_UP_TO_DATE request to client");
            }
            free_request(&file_up_to_date_request);
        } else {
            request file_request = create_file_request(file, current_version);
            if (ipv4_socket_send_request(&connected_client->socket, file_request) < 0) {
                report_error("Couldn't send file requested to client");
            }
        }
        free(file.data);
    }
    close(connected_client->socket.socket_fd);
    connected_client->socket.socket_fd = -1;
}

static void
handle_request(request_handler_arguments *arguments, connected_client *connected_client, request request) {
    u32 command_length = request.header.command_length;
    byte *data = request.data;
    list *connected_clients = arguments->connected_clients;

    if (data == NULL) return;

    if (str_n_equals(data, LOG_ON, command_length)) {
        handle_log_on_request(&connected_client->socket, data + command_length, connected_clients);
    } else if (str_n_equals(data, GET_CLIENTS, command_length)) {
        handle_get_clients_request(connected_client, connected_clients);
    } else if (str_n_equals(data, LOG_OFF, command_length)) {
        handle_log_off_request(connected_client, connected_clients);
    } else if (str_n_equals(data, GET_FILE, command_length)) {
        handle_get_file_request(connected_client, data + command_length, request.header.bytes - command_length);
    } else if (str_n_equals(data, GET_FILE_LIST, command_length)) {
        handle_get_file_list_request(connected_client, arguments->directory_name);
    } else if (str_n_equals(data, USER_ON, command_length)) {
        handle_user_on_request(connected_client, data + command_length, connected_clients, arguments->shared_buffer);
    } else if (str_n_equals(data, USER_OFF, command_length)) {
        handle_user_off_request(connected_client, connected_clients, data + command_length);
    } else {
        report_response("Unknown command! (%.*s)", command_length, data);
    }
}

static void
serve_client(request_handler_arguments *arguments, connected_client *connected_client) {
    request request = ipv4_socket_get_request(&connected_client->socket);
    if (IS_EMPTY(request)) {
        return;
    }
    handle_request(arguments, connected_client, request);
    free_request(&request);
}

void handle_incoming_requests(request_handler_arguments *arguments) {
    ipv4_socket *server_socket = arguments->server_socket;
    list *connected_clients = arguments->connected_clients;

    if (FD_ISSET(server_socket->socket_fd, arguments->set)) {
        ipv4_socket client_socket = {0};
        if (ipv4_socket_accept(server_socket, &client_socket) < 0) {
            report_error("Couldn't accept an incoming connection!");
        }
        report_response("Accepted connection!");
        connected_client new_client = {
                .socket = client_socket,
        };
        serve_client(arguments, &new_client);
    }

    list_node *curr = connected_clients->head;
    if (curr != NULL) {
        do {
            connected_client *connected_client = curr->data;
            if (connected_client->socket.socket_fd != -1 &&
                FD_ISSET(connected_client->socket.socket_fd, arguments->set)) {

                serve_client(arguments, connected_client);
            }
            curr = curr->next;
        } while (curr != connected_clients->head);
    }
}

char *get_self_ip_address(void) {
    char name_buffer[256];
    gethostname(name_buffer, sizeof(name_buffer));
    struct hostent *host_enrty = gethostbyname(name_buffer);
    char *ip = inet_ntoa(*((struct in_addr *) host_enrty->h_addr));
    return ip;
}