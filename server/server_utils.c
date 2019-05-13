#include <stdbool.h>
#include <stdlib.h>
#include "server_utils.h"
#include "../common/string_utils.h"
#include "../common/report_utils.h"

void usage(void) {
    fprintf(stderr,
            "Server usage:\n"
            "./dropbox_server\n"
            "\t-p <port_number>\n");
    exit(EXIT_SUCCESS);
}

server_options parse_command_line_arguments(int argc, char *argv[]) {
    server_options options = {0};
    int option;
    while (true) {
        int option_index;
        option = getopt_long_only(argc, argv, "p:", options_spec, &option_index);
        if (option == -1) break;
        switch (option) {
            case 'p': {
                u64 value;
                if (!str_to_ui64(optarg, &value)) {
                    die("There was an error while reading the port number."
                        "Please make sure you have provided a valid port number.");
                }
                if (value < 2000) {
                    die("You must specify a socket number greater or equal to 2000.");
                }
                options.port_number = (u16) value;
                break;
            }

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