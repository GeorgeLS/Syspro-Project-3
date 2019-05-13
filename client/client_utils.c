#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "client_utils.h"
#include "../Common/string_utils.h"
#include "../Common/report_utils.h"

void usage(void) {
    fprintf(stderr,
            "Client usage:\n"
            "./dropbox_client\n"
            "\t-d <directory_name>\n"
            "\t-p <port_number>\n"
            "\t-w <worker_threads>\n"
            "\t-b <buffer_size>\n"
            "\t-sp <server_port>\n"
            "\t-sip <server_ip>\n");
    exit(EXIT_SUCCESS);
}

client_options parse_command_line_arguments(int argc, char *argv[]) {
    client_options options = {0};
    int option;
    while (true) {
        int option_index;
        option = getopt_long_only(argc, argv, "d:p:w:b:s:i:", options_spec, &option_index);
        if (option == -1) break;
        switch (option) {
            case 'd':
                options.directory_name = strdup(optarg);
                break;

            case 'p': {
                u64 value;
                if (!str_to_ui64(optarg, &value)) {
                    die("There was an error while reading the port number."
                        "Please make sure you provide a valid port number.");
                }
                options.port_number = (u16) value;
                break;
            }

            case 'w': {
                u64 value;
                if (!str_to_ui64(optarg, &value)) {
                    die("There was an error while reading the number of worker threads."
                        "Please make sure you provide a valid number of worker threads.");
                }
                options.worker_threads = (u16) value;
                break;
            }

            case 'b':
                if (!str_to_ui64(optarg, &options.buffer_size)) {
                    die("There was an error while reading the buffer size."
                        "Please make sure you provide a valid buffer size.");
                }
                break;

            case 's': {
                u64 value;
                if (!str_to_ui64(optarg, &value)) {
                    die("There was an error while reading the server port number."
                        "Please make sure you provide a valid server port number.");
                }
                options.server_port_number = (u16) value;
                break;
            }

            case 'i':
                ipv4_initialize(&options.server_ip, optarg);
                break;

            case 'h':
                usage();

            case '?':
                break;

            default:
                abort();
        }
    }
    return options;
}