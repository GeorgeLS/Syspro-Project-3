#ifndef EXERCISE_III_COMMANDS_H
#define EXERCISE_III_COMMANDS_H

#include <stddef.h>
#include <netinet/in.h>
#include "common/attributes.h"
#include "common/common_types.h"
#include "generic_multithreaded_list.h"

struct ipv4_socket;

#define LOG_ON "LOG_ON"
#define USER_ON "USER_ON"
#define GET_CLIENTS "GET_CLIENTS"
#define LOG_OFF "LOG_OFF"
#define USER_OFF "USER_OFF"
#define CLIENT_LIST "CLIENT_LIST"
#define GET_FILE_LIST "GET_FILE_LIST"
#define GET_FILE "GET_FILE"
#define FILE_LIST "FILE_LIST"

#define __COMMAND_LENGTH(command) __##command##_LENGTH
#define __LOG_ON_LENGTH 6
#define __USER_ON_LENGTH 7
#define __LOG_OFF_LENGTH 7
#define __USER_OFF_LENGTH 8
#define __GET_CLIENTS_LENGTH 11
#define __GET_FILE_LIST_LENGTH 13
#define __GET_FILE_LENGTH 8
#define __CLIENT_LIST_LENGTH 10
#define __FILE_LIST_LENGTH 9

typedef struct request_header {
    u32 bytes;
    u32 command_length;
} request_header;

typedef struct request {
    request_header header;
    byte *data;
} request;

static __INLINE__
request_header header_hton(request_header header) {
    return (request_header) {
            .bytes = htonl(header.bytes),
            .command_length = htonl(header.command_length)
    };
}

static __INLINE__
request_header header_ntoh(request_header header) {
    return (request_header) {
            .bytes = ntohl(header.bytes),
            .command_length = ntohl(header.command_length)
    };
}

__NON_NULL__(1)
void free_request(request *request);

__NON_NULL__(2)
request create_log_on_request(u16 port_number, const char *restrict address);

__NON_NULL__(2)
request create_user_on_request(u16 port_number, const char *restrict address);

request create_log_off_request(void);

__NON_NULL__(2)
request create_user_off_request(u16 port_number, const char *restrict address);

request create_get_clients_request(void);

__NON_NULL__(1)
request create_client_list_request(list *list);

request create_get_file_list_request(void);

__NON_NULL__(1)
request create_file_list_request(const char *root_directory);

#endif //EXERCISE_III_COMMANDS_H
