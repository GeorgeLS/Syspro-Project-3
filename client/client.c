#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <inttypes.h>
#include <zconf.h>
#include <string.h>
#include <arpa/inet.h>
#include "client_utils.h"
#include "../socket/ipv4_socket.h"
#include "../common/report_utils.h"
#include "../commands.h"
#include "../client_list.h"
#include "../common/string_utils.h"

client_options options;
ipv4_socket client_socket;

client_list other_clients;

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

int main(int argc, char *argv[]) {
    if (argc < 13) usage();
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
        for (byte *address = request.data + request.header.command_length;
             address < request.data + request.header.bytes;
             address += IPV4_ADDRESS_SIZE) {
            u32 binary_ip = *((u32 *) address);
            char ip[MAX_IPV4_LENGTH];
            inet_ntop(AF_INET, &binary_ip, ip, MAX_IPV4_LENGTH);
            u16 port_number = ntohs(*((u16 *) (address + sizeof(u32))));
            report_response("IP: %.*s\nPort: %" PRIu16, MAX_IPV4_LENGTH, ip, port_number);
        }
    }

    close(client_socket.socket_fd);
    return EXIT_SUCCESS;
}