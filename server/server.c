#include <stdlib.h>
#include <stdio.h>
#include "server_utils.h"

int main(int argc, char *argv[]) {
    if (argc < 3) usage();
    server_options options = parse_command_line_arguments(argc, argv);
    return EXIT_SUCCESS;
}