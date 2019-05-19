#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "common_types.h"
#include "file_utils.h"
#include "macros.h"

int read_line_from_stdin(void *restrict stdin_buffer, size_t bytes) {
    return read(STDIN_FILENO, stdin_buffer, bytes);
}

entire_file read_entire_file_into_memory(const char *filename) {
    entire_file file = {0};
    struct stat info = {0};
    if (stat(filename, &info) == -1) return file;
    size_t length = info.st_size;
    void *data = __MALLOC__(length + 1, byte);
    if (data == NULL) return file;
    int fd = open(filename, O_RDONLY);
    if (fd == -1) return file;
    if (read(fd, data, length) == -1) return file;
    file.data = data;
    file.size = length;
    return file;
}

bool get_all_pathnames_and_versions(const char *restrict root_directory,
                                    versioned_pathname **pathnames_out,
                                    size_t *pathnames_n_out) {
    return false;
}