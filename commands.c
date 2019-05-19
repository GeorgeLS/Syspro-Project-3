#include "commands.h"
#include "common/macros.h"
#include "ipv4.h"
#include "socket/ipv4_socket.h"
#include "common/file_utils.h"
#include "shared_client_buffer.h"
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void free_request(request *request) {
    free(request->data);
}

request create_log_on_request(u16 port_number, const char *restrict address) {
    request_header header = {
            .command_length = __COMMAND_LENGTH(LOG_ON),
            .bytes = __COMMAND_LENGTH(LOG_ON) + IPV4_ADDRESS_SIZE
    };
    byte *data = __MALLOC__(header.bytes, byte);
    byte *base_address = data;
    memcpy(data, LOG_ON, header.command_length);
    data += header.command_length;
    in_addr_t address_bytes = inet_addr(address);
    memcpy(data, &address_bytes, sizeof(in_addr_t));
    data += sizeof(in_addr_t);
    u16 port_number_network_order = htons(port_number);
    memcpy(data, &port_number_network_order, sizeof(u16));
    return (request) {
            .header = header_hton(header),
            .data = base_address
    };
}

request create_get_clients_request(void) {
    request_header header = {
            .command_length = __COMMAND_LENGTH(GET_CLIENTS),
            .bytes = __COMMAND_LENGTH(GET_CLIENTS)
    };
    byte *data = __MALLOC__(header.bytes, byte);
    memcpy(data, GET_CLIENTS, header.command_length);
    return (request) {
            .header = header_hton(header),
            .data = data
    };
}

request create_log_off_request(void) {
    request_header header = {
            .command_length = __COMMAND_LENGTH(LOG_OFF),
            .bytes = __COMMAND_LENGTH(LOG_OFF)
    };
    byte *data = __MALLOC__(header.bytes, byte);
    memcpy(data, LOG_OFF, header.command_length);
    return (request) {
            .header = header_hton(header),
            .data = data
    };
}

request create_client_list_request(client_list *list) {
    request_header header = {
            .command_length = __COMMAND_LENGTH(CLIENT_LIST),
            .bytes = __COMMAND_LENGTH(CLIENT_LIST) + (list->size * IPV4_ADDRESS_SIZE)
    };
    byte *data = __MALLOC__(header.bytes, byte);
    byte *base_address = data;
    memcpy(data, CLIENT_LIST, header.command_length);
    data += header.command_length;
    list_node *curr = list->head;
    do {
        u32 binary_ip = inet_addr(curr->tuple.ip);
        u16 port_number = htons(curr->tuple.port_number);
        memcpy(data, &binary_ip, sizeof(u32));
        data += sizeof(u32);
        memcpy(data, &port_number, sizeof(u16));
        data += sizeof(u16);
        curr = curr->next;
    } while (curr != list->head);
    return (request) {
            .data = base_address,
            .header = header_hton(header)
    };
}

request create_get_file_list_request(void) {
    request_header header = {
            .bytes = __COMMAND_LENGTH(GET_FILE_LIST),
            .command_length = __COMMAND_LENGTH(GET_FILE_LIST)
    };

    byte *data = __MALLOC__(header.bytes, byte);

    memcpy(data, GET_FILE_LIST, header.command_length);

    return (request) {
            .header = header_hton(header),
            .data = data
    };
}

request create_file_list_request(const char *root_directory) {
    versioned_pathname *pathnames;
    size_t pathnames_n;
    get_all_pathnames_and_versions(root_directory, &pathnames, &pathnames_n);
    request_header header = {
            .bytes = __COMMAND_LENGTH(FILE_LIST) + (pathnames_n * (MAX_PATHNAME_SIZE + sizeof(u64))),
            .command_length = __COMMAND_LENGTH(FILE_LIST)
    };

    byte *data = __MALLOC__(header.bytes, byte);
    byte *base_address = data;

    memcpy(data, FILE_LIST, header.command_length);
    data += header.command_length;

    for (size_t i = 0U; i != pathnames_n; ++i) {
        memcpy(data, pathnames[i].pathname, MAX_PATHNAME_SIZE);
        data += MAX_PATHNAME_SIZE;
        memcpy(data, &pathnames[i].version, sizeof(u64));
        data += sizeof(u64);
    }

    return (request) {
            .data = base_address,
            .header = header_hton(header)
    };
}