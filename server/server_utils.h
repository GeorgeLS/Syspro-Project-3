#ifndef EXERCISE_III_SERVER_UTILS_H
#define EXERCISE_III_SERVER_UTILS_H

#include <stdio.h>
#include <getopt.h>
#include "../Common/attributes.h"
#include "../Common/common_types.h"

typedef struct server_options {
    u16 port_number;
} server_options;

static struct option options_spec[] = {
        {"p", required_argument, NULL, 'p'},
        {0, 0, 0,                      0}
};

__NO_RETURN__
void usage(void);

__NON_NULL__(2)
server_options parse_command_line_arguments(int argc, char *argv[]);

#endif //EXERCISE_III_SERVER_UTILS_H
