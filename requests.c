#include "requests.h"
#include "common/macros.h"
#include "socket/ipv4_socket.h"
#include "common/file_utils.h"
#include "shared_client_buffer.h"
#include "client/client_data.h"
#include "common/common_data.h"
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <bits/types/FILE.h>

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
            .header = header,
            .data = base_address
    };
}

request create_user_on_request(u16 port_number, const char *restrict address) {
    request_header header = {
            .command_length = __COMMAND_LENGTH(USER_ON),
            .bytes = __COMMAND_LENGTH(USER_ON) + IPV4_ADDRESS_SIZE
    };

    byte *data = __MALLOC__(header.bytes, byte);
    byte *base_address = data;

    memcpy(data, USER_ON, header.command_length);
    data += header.command_length;

    in_addr_t address_bytes = inet_addr(address);
    memcpy(data, &address_bytes, sizeof(in_addr_t));
    data += sizeof(in_addr_t);

    u16 port_number_network_order = htons(port_number);
    memcpy(data, &port_number_network_order, sizeof(u16));

    return (request) {
            .header = header,
            .data = base_address
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
            .header = header,
            .data = data
    };
}

request create_user_off_request(u16 port_number, const char *restrict address) {
    request_header header = {
            .command_length = __COMMAND_LENGTH(USER_OFF),
            .bytes = __COMMAND_LENGTH(USER_OFF) + IPV4_ADDRESS_SIZE
    };

    byte *data = __MALLOC__(header.bytes, byte);
    byte *base_address = data;

    memcpy(data, USER_OFF, header.command_length);
    data += header.command_length;

    in_addr_t address_bytes = inet_addr(address);
    memcpy(data, &address_bytes, sizeof(in_addr_t));
    data += sizeof(in_addr_t);

    u16 port_number_network_order = htons(port_number);
    memcpy(data, &port_number_network_order, sizeof(u16));

    return (request) {
            .header = header,
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
            .header = header,
            .data = data
    };
}

request create_client_list_request(list *list, connected_client *receiver) {
    request_header header = {
            .command_length = __COMMAND_LENGTH(CLIENT_LIST),
            .bytes = __COMMAND_LENGTH(CLIENT_LIST) + ((list->size - 1) * IPV4_ADDRESS_SIZE)
    };

    byte *data = __MALLOC__(header.bytes, byte);
    byte *base_address = data;

    memcpy(data, CLIENT_LIST, header.command_length);
    data += header.command_length;

    list_node *curr = list->head;
    do {
        connected_client *connected_client = curr->data;
        if (!connected_client_equals(connected_client, receiver)) {
            u32 binary_ip = inet_addr(connected_client->tuple.ip);
            u16 port_number = htons(connected_client->tuple.port_number);

            memcpy(data, &binary_ip, sizeof(u32));
            data += sizeof(u32);

            memcpy(data, &port_number, sizeof(u16));
            data += sizeof(u16);
        }

        curr = curr->next;
    } while (curr != list->head);

    return (request) {
            .data = base_address,
            .header = header
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
            .header = header,
            .data = data
    };
}

request create_file_list_request(const char *restrict root_directory) {
    list versioned_pathnames = list_create(NULL, !LIST_MULTITHREADED);
    get_all_pathnames_and_versions(root_directory, &versioned_pathnames);
    request_header header = {
            .bytes = __COMMAND_LENGTH(FILE_LIST) + (versioned_pathnames.size * (MAX_PATHNAME_SIZE + sizeof(u64))),
            .command_length = __COMMAND_LENGTH(FILE_LIST)
    };

    byte *data = __MALLOC__(header.bytes, byte);
    byte *base_address = data;

    memcpy(data, FILE_LIST, header.command_length);
    data += header.command_length;

    list_node *curr = versioned_pathnames.head;
    do {
        versioned_pathname *vpathname = curr->data;
        memcpy(data, vpathname->pathname, MAX_PATHNAME_SIZE);
        data += MAX_PATHNAME_SIZE;
        memcpy(data, &vpathname->version, sizeof(u64));
        data += sizeof(u64);
    } while (curr != versioned_pathnames.head);

    return (request) {
            .data = base_address,
            .header = header
    };
}

request create_get_file_request(versioned_pathname *vpathname) {
    request_header header = {
            .command_length = __COMMAND_LENGTH(GET_FILE),
            .bytes = __COMMAND_LENGTH(GET_FILE) + sizeof(versioned_pathname)
    };

    byte *data = __MALLOC__(header.bytes, byte);
    byte *base_address = data;

    memcpy(data, GET_FILE, header.command_length);
    data += header.command_length;

    memcpy(data, vpathname, sizeof(versioned_pathname));

    return (request) {
            .header = header,
            .data = base_address
    };
}

request create_file_not_found_request(void) {
    request_header header = {
            .bytes = __COMMAND_LENGTH(FILE_NOT_FOUND),
            .command_length = __COMMAND_LENGTH(FILE_NOT_FOUND)
    };

    byte *data = __MALLOC__(header.bytes, byte);
    memcpy(data, FILE_NOT_FOUND, header.command_length);

    request result = {
            .data = data,
            .header = header
    };

    return result;
}

request create_file_up_to_date_request(void) {
    request_header header = {
            .bytes = __COMMAND_LENGTH(FILE_UP_TO_DATE),
            .command_length = __COMMAND_LENGTH(FILE_UP_TO_DATE)
    };

    byte *data = __MALLOC__(header.bytes, byte);
    memcpy(data, FILE_UP_TO_DATE, header.command_length);

    request result = {
            .data = data,
            .header = header
    };

    return result;
}