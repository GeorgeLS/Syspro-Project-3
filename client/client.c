#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <inttypes.h>
#include <zconf.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>
#include "client_utils.h"
#include "../socket/ipv4_socket.h"
#include "../common/report_utils.h"
#include "../commands.h"
#include "../client_list.h"
#include "../common/string_utils.h"
#include "../shared_client_buffer.h"

client_options options;

ipv4_socket client_socket;

client_list other_clients;

shared_buffer info_buffer;

void request_files_from_cient();

void setup_client_socket() {
    if (ipv4_socket_create(options.port_number, IPV4_ANY_ADDRESS, &client_socket) < 0) {
        die("Could not create client address!");
    }
}

ipv4_socket connect_to_server(void) {
    ipv4_socket server_socket;

    if (ipv4_socket_create(options.server_port_number, (struct in_addr) {inet_addr(options.server_ip.address)},
                           &server_socket) < 0) {
        die("Couldn't create server socket");
    }

    if (ipv4_socket_connect(&server_socket) < 0) {
        die("Couldn't connect to the server!");
    }

    report_response("Connected to server %s with port %" PRIu16, options.server_ip.address, options.server_port_number);
    return server_socket;
}

void add_clients_to_list(request request) {
    for (byte *address = request.data + request.header.command_length;
         address < request.data + request.header.bytes;
         address += IPV4_ADDRESS_SIZE) {
        u32 binary_ip = *((u32 *) address);
        char ip[MAX_IPV4_LENGTH];
        inet_ntop(AF_INET, &binary_ip, ip, MAX_IPV4_LENGTH);
        u16 port_number = ntohs(*((u16 *) (address + sizeof(u32))));
        client_tuple tuple = {
                .ip = ip,
                .port_number = port_number};
        client_list_rpush(&other_clients, &tuple);
    }
}

bool connect_to(client_tuple *tuple, ipv4_socket *socket_out) {
    ipv4_socket socket;
    u32 binary_ip = inet_addr(tuple->ip);
    if (ipv4_socket_create(tuple->port_number, (struct in_addr) {binary_ip}, &socket) < 0) {
        report_error("Couldn't create new socket to connect to client with"
                     "I.P: %s and Port: %" PRIu16,
                     tuple->ip, tuple->port_number);
        return false;
    }
    if (ipv4_socket_connect(&socket) < 0) {
        report_error("Couldn't connect to client with I.P: %s and Port: %" PRIu16,
                     tuple->ip, tuple->port_number);
        return false;
    }
    return true;
}

bool request_files_from_client(client_file_info *info) {
    client_tuple tuple = {
            .ip = info->tuple.ip,
            .port_number = info->tuple.port_number};

    if (!client_list_exists(&other_clients, &tuple)) {
        return false;
    }

    ipv4_socket client_to_connect;
    if (!connect_to(&tuple, &client_to_connect)) {
        report_error("Couldn't connect to the client with I.P: %s and Port: %" PRIu16,
                     tuple.ip, tuple.port_number);
        return false;
    }
    request request = create_get_file_list_request();
    if (ipv4_socket_send_request(&client_to_connect, request) < 0) {
        report_error("Couldn't send GET_FILE_LIST request to client with"
                     "I.P: %s and Port: %" PRIu16,
                     tuple.ip, tuple.port_number);
    }
    free_request(&request);
    client_file_info client_info = {
            .tuple = {
                    .ip = info->tuple.ip,
                    .port_number = info->tuple.port_number
            }
    };
    request = get_request(&client_to_connect);

    for (byte *address = request.data + request.header.command_length;
         address < request.data + request.header.bytes;
         address += sizeof(versioned_pathname)) {
        client_info.pathname_with_version = *((versioned_pathname *) address);
        shared_buffer_push(&info_buffer, &client_info);
    }

    free_request(&request);

    return true;
}

void *worker_function(void *args) {
    while (true) {
        while (shared_buffer_emtpy(&info_buffer)) {
//            sleep(2);
        }

        client_file_info *info = shared_buffer_pop(&info_buffer);

        if (!client_file_info_contains_file(info)) {
            if (!request_files_from_client(info)) {
                break;
            }
        } else {

        }
    }
    return NULL;
}

void create_threads(pthread_t *thread_pool) {
    for (size_t i = 0U; i != options.worker_threads; ++i) {
        if (pthread_create(&thread_pool[i], NULL, worker_function, NULL)) {
            die("Couldn't create worker thread!");
        }
    }
}

void wait_threads(pthread_t *thread_pool) {
    for (size_t i = 0U; i != options.worker_threads; ++i) {
        if (pthread_join(thread_pool[i], NULL)) {
            die("Couldn't join threads");
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 13)
        usage();
    options = parse_command_line_arguments(argc, argv);
    other_clients = client_list_create(CLIENT_LIST_MULTITHREADED);

    //    setup_client_socket();

    ipv4_socket server_socket = connect_to_server();

    request request;
    request = create_log_on_request(options.port_number, options.server_ip.address);
    if (ipv4_socket_send_request(&server_socket, request) < 0) {
        report_error("Couldn't send LOG_ON request to server!");
    }
    free_request(&request);

    request = create_get_clients_request();
    if (ipv4_socket_send_request(&server_socket, request) < 0) {
        report_error("Couldn't send GET_CLIENTS request to server");
    }
    free_request(&request);

    request = get_request(&server_socket);

    if (str_n_equals(request.data, CLIENT_LIST, request.header.command_length)) {
        add_clients_to_list(request);
    }

    info_buffer = shared_buffer_create(options.buffer_size);

    // we assume that the number of threads won't
    // be that large so we allocate memory on the stack
    pthread_t *thread_pool = alloca(options.worker_threads * sizeof(pthread_t));

    create_threads(thread_pool);

    wait_threads(thread_pool);

    close(client_socket.socket_fd);
    return EXIT_SUCCESS;
}