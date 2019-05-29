#ifndef EXERCISE_III_COMMON_UTILS_H
#define EXERCISE_III_COMMON_UTILS_H

#include "../generic_multithreaded_list.h"
#include "../socket/ipv4_socket.h"
#include "common_data.h"

__NON_NULL__(1, 3)
void reset_and_add_socket_descriptors_to_set(fd_set *set, int server_fd, list *_list);

__NON_NULL__(1, 2, 3)
void handle_incoming_requests(fd_set *set, ipv4_socket *server_socket, list *connected_clients);

__NON_NULL__(1, 2)
void handle_request(list *connected_clients, connected_client *connected_client, request request);

__NON_NULL__(1, 2)
void serve_client(list *connected_clients, connected_client *client);

__NON_NULL__(1, 2, 3)
void handle_log_on_request(ipv4_socket *sender, byte *data, list *connected_clients);

__NON_NULL__(1, 2)
void handle_get_clients_request(connected_client *connected_client, list *connected_clients);

__NON_NULL__(1, 2)
void handle_log_off_request(connected_client *client, list *connected_clients);

#endif //EXERCISE_III_COMMON_UTILS_H
