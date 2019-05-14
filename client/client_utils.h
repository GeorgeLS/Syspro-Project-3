#ifndef EXERCISE_III_CLIENT_UTILS_H
#define EXERCISE_III_CLIENT_UTILS_H

#include <stddef.h>
#include <getopt.h>
#include "../ipv4.h"
#include "../socket/ipv4_socket.h"

static struct option options_spec[] = {
        {"d", required_argument, NULL, 'd'},
        {"p", required_argument, NULL, 'p'},
        {"w", required_argument, NULL, 'w'},
        {"b", required_argument, NULL, 'b'},
        {"sp", required_argument, NULL, 's'},
        {"sip", required_argument, NULL, 'i'},
        {0, 0, 0, 0}
};

typedef struct client_options {
    ipv4 server_ip;
    char *directory_name;
    size_t buffer_size;
    u16 port_number;
    u16 server_port_number;
    u16 worker_threads;
} client_options;

__NO_RETURN__
void usage(void);

__NON_NULL__(2)
client_options parse_command_line_arguments(int argc, char *argv[]);

#endif //EXERCISE_III_CLIENT_UTILS_H
