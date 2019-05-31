#include <stdio.h>
#include <inttypes.h>
#include "generic_multithreaded_list.h"
#include "common/file_utils.h"
#include "client/client_data.h"
#include "requests.h"

int main(void) {
    request _req = create_file_list_request("input");
    return 0;
}