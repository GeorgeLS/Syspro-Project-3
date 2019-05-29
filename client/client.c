#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <inttypes.h>
#include <zconf.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "client_utils.h"
#include "../socket/ipv4_socket.h"
#include "../common/report_utils.h"
#include "../commands.h"
#include "../common/string_utils.h"
#include "../shared_client_buffer.h"
#include "../common/macros.h"
#include "../common/file_utils.h"
#include "../common/common_data.h"
#include "../common/common_utils.h"

client_options options;

ipv4_socket self_socket;
ipv4_socket server_socket;

list connected_clients;
shared_buffer info_buffer;

pthread_t *threads;

void setup_client_socket() {
    if (ipv4_socket_create(options.port_number, IPV4_ANY_ADDRESS, &self_socket) < 0) {
        die("Could not create client address!");
    }
    if (ipv4_socket_bind(&self_socket) < 0) {
        die("Could not bind client socket!");
    }
    if (ipv4_socket_listen(&self_socket) < 0) {
        die("Could not set client socket for listening!");
    }
}

ipv4_socket connect_to_server(void) {
    if (ipv4_socket_create(options.server_port_number,
                           (struct in_addr) {inet_addr(options.server_ip)},
                           &server_socket) < 0) {
        die("Couldn't create server socket");
    }

    if (ipv4_socket_connect(&server_socket) < 0) {
        die("Couldn't connect to the server!");
    }

    report_response("Connected to server %s with port %" PRIu16, options.server_ip, options.server_port_number);
    return server_socket;
}


bool request_files_from_client(ipv4_socket *client_socket, client_tuple *tuple) {
    request request = create_get_file_list_request();
    if (ipv4_socket_send_request(client_socket, request) < 0) {
        report_error("Couldn't send GET_FILE_LIST request to client with"
                     "I.P: %s and Port: %" PRIu16,
                     tuple->ip, tuple->port_number);
    }
    free_request(&request);
    client_file_info client_info = {
            .tuple = *tuple
    };
    request = ipv4_socket_get_request(client_socket);

    for (byte *address = request.data + request.header.command_length;
         address < request.data + request.header.bytes;
         address += sizeof(versioned_pathname)) {
        client_info.pathname_with_version = *((versioned_pathname *) address);
        shared_buffer_push(&info_buffer, &client_info);
    }

    free_request(&request);
    close(client_socket->socket_fd);

    return true;
}

bool request_file_from_client(ipv4_socket *client_socket, client_file_info *info) {
    bool result = true;
    request request = {0};
    char *full_pathname;
    asprintf(&full_pathname, "%s_%" PRIu16 "/%s", info->tuple.ip, info->tuple.port_number,
             info->pathname_with_version.pathname);

    if (!file_exists(full_pathname)) {
        info->pathname_with_version.version = 0U;
        if (!create_directory(full_pathname, ACCESSPERMS)) {
            report_error("Couldn't create new directory!");
            result = false;
            goto __ERROR__;
        }
    }

    request = create_get_file_request(&info->pathname_with_version);
    if (!ipv4_socket_send_request(client_socket, request)) {
        report_error("Couldn't send GET_FILE request to client");
        result = false;
        goto __ERROR__;
    }
    free_request(&request);

    request = ipv4_socket_get_request(client_socket);
    if (!str_n_equals(request.data + request.header.command_length,
                      FILE_UP_TO_DATE,
                      request.header.bytes)) {
        result = false;
        goto __ERROR__;
    }

    int fd;
    if ((fd = open(full_pathname, O_CREAT | O_WRONLY)) < 0) {
        report_error("Couldn't open file %s for write", full_pathname);
        result = false;
        goto __ERROR__;
    }

    write(fd, request.data, request.header.bytes);

    close(fd);

    __ERROR__:
    free(full_pathname);
    free_request(&request);
    return result;
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

void *worker_function(void *args) {
    while (true) {
        client_file_info *info;
        while ((info = shared_buffer_pop(&info_buffer)) == NULL) {
//            sleep(2);
        }

        client_tuple tuple = {
                .ip = info->tuple.ip,
                .port_number = info->tuple.port_number
        };

        if (!list_element_exists(&connected_clients, &tuple)) {
            continue;
        }

        ipv4_socket client_socket;
        if (!ipv4_socket_create_and_connect(&tuple, &client_socket)) {
            continue;
        }

        if (!client_file_info_contains_file(info)) {
            request_files_from_client(&client_socket, &tuple);
        } else {
            request_file_from_client(&client_socket, info);
        }
        close(client_socket.socket_fd);
    }
    pthread_exit(NULL);
}

#pragma clang diagnostic pop

void create_threads(void) {
    for (size_t i = 0U; i != options.worker_threads; ++i) {
        if (pthread_create(&threads[i], NULL, worker_function, NULL)) {
            die("Couldn't create worker thread!");
        }
    }
}

void wait_threads(void) {
    for (size_t i = 0U; i != options.worker_threads; ++i) {
        if (pthread_join(threads[i], NULL)) {
            die("Couldn't join threads");
        }
    }
}

void add_clients_to_list_and_buffer(request request) {
    for (byte *address = request.data + request.header.command_length;
         address < request.data + request.header.bytes;
         address += IPV4_ADDRESS_SIZE) {

        client_file_info *info = __MALLOC__(1, client_file_info);
        info->tuple = client_tuple_from_ntoh_bytes(address);
        info->pathname_with_version = (versioned_pathname) {0};

        connected_client *client = connected_client_create(&info->tuple, &(ipv4_socket) {.socket_fd = -1});

        list_rpush(&connected_clients, client);
        shared_buffer_push(&info_buffer, info);
    }
}

void get_other_clients_from_server(void) {
    request request;
    request = create_log_on_request(options.port_number, options.server_ip);
    if (ipv4_socket_send_request(&server_socket, request) < 0) {
        report_error("Couldn't send LOG_ON request to server!");
    }
    free_request(&request);

    request = create_get_clients_request();
    if (ipv4_socket_send_request(&server_socket, request) < 0) {
        report_error("Couldn't send GET_CLIENTS request to server");
    }
    free_request(&request);

    request = ipv4_socket_get_request(&server_socket);
    add_clients_to_list_and_buffer(request);
    free_request(&request);
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

int main(int argc, char *argv[]) {
    if (argc < 13) usage();
    options = parse_command_line_arguments(argc, argv);
    connected_clients = list_create(connected_client_equals, LIST_MULTITHREADED);
    info_buffer = shared_buffer_create(options.buffer_size);

    //    setup_client_socket();

    server_socket = connect_to_server();
    get_other_clients_from_server();

    // we assume that the number of threads won't
    // be that large so we allocate memory on the stack
    threads = __MALLOC__(options.worker_threads, pthread_t);

    create_threads();

    fd_set sockets_set;
    while (true) {
        reset_and_add_socket_descriptors_to_set(&sockets_set, server_socket.socket_fd, &connected_clients);
        bool available_for_read = select(FD_SETSIZE, &sockets_set, NULL, NULL, NULL) > 0;
        if (available_for_read) {
            handle_incoming_requests(&sockets_set, &server_socket, &connected_clients);
        }
    }

    wait_threads();
    return EXIT_SUCCESS;
}

#pragma clang diagnostic pop