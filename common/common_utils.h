#ifndef EXERCISE_III_COMMON_UTILS_H
#define EXERCISE_III_COMMON_UTILS_H

#include "../generic_multithreaded_list.h"
#include "../socket/ipv4_socket.h"
#include "common_data.h"

typedef struct request_handler_arguments {
    fd_set *set;
    ipv4_socket *server_socket;
    list *connected_clients;
    char *directory_name;
    shared_buffer *shared_buffer;
} request_handler_arguments;

__NON_NULL__(1, 3)
void reset_and_add_socket_descriptors_to_set(fd_set *set, int server_fd, list *connected_clients);

__NON_NULL__(1)
void handle_incoming_requests(request_handler_arguments *arguments);

char *get_self_ip_address(void);

#endif //EXERCISE_III_COMMON_UTILS_H
