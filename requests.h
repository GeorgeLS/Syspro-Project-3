#ifndef EXERCISE_III_REQUESTS_H
#define EXERCISE_III_REQUESTS_H

#include <stddef.h>
#include <netinet/in.h>
#include "common/attributes.h"
#include "common/common_types.h"
#include "generic_multithreaded_list.h"
#include "shared_client_buffer.h"
#include "common/file_utils.h"

struct ipv4_socket;

#define FILE_UP_TO_DATE "FILE_UP_TO_DATE"
#define FILE_NOT_FOUND "FILE_NOT_FOUND"

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
#define __CLIENT_LIST_LENGTH 11
#define __FILE_LIST_LENGTH 9
#define __FILE_UP_TO_DATE_LENGTH 15
#define __FILE_NOT_FOUND_LENGTH 14

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

__NON_NULL__(2)
request create_log_off_request(u16 port_number, const char *restrict address);

__NON_NULL__(2)
request create_user_off_request(u16 port_number, const char *restrict address);

request create_get_clients_request(void);

struct connected_client;

__NON_NULL__(1, 2)
request create_client_list_request(list *list, struct connected_client *receiver);

request create_get_file_list_request(void);

__NON_NULL__(1)
request create_file_list_request(const char *restrict root_directory);

__NON_NULL__(1)
request create_get_file_request(versioned_pathname *vpathname);

request create_file_not_found_request(void);

request create_file_up_to_date_request(void);

request create_file_request(entire_file file, u64 version);


#endif //EXERCISE_III_REQUESTS_H
