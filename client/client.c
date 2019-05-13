#include <stdlib.h>
#include <stdio.h>
#include "client_utils.h"

int main(int argc, char *argv[]) {
    if (argc < 13) usage();
    client_options options = parse_command_line_arguments(argc, argv);
    return EXIT_SUCCESS;
}