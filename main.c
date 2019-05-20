#include <stdio.h>
#include <inttypes.h>
#include "generic_multithreaded_list.h"
#include "common/file_utils.h"
#include "client/client_data.h"

int main() {
    const char *root_directory = "input";
    list vpathnames = list_create(NULL, !LIST_MULTITHREADED);
    get_all_pathnames_and_versions(root_directory, &vpathnames);
    list_node *curr = vpathnames.head;
    do {
        versioned_pathname *vpathname = curr->data;
        printf("Pathname: %s\n"
               "Version: %" PRIu64 "\n\n",
               vpathname->pathname, vpathname->version);
        curr = curr->next;
    } while (curr != vpathnames.head);
    list_destroy(&vpathnames);
    return 0;
}