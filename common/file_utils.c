#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include "common_types.h"
#include "file_utils.h"
#include "macros.h"

u64 compute_file_hash(entire_file file) {
    u64 hash_result = 0U;
    size_t double_words = file.size / sizeof(u64);
    size_t remaining_bytes = file.size % sizeof(u64);
    for (size_t i = 0U; i != double_words; ++i) {
        hash_result += *(u64 *) file.data;
        file.data += sizeof(u64);
    }
    for (size_t i = 0U; i != remaining_bytes; ++i) {
        hash_result += *(byte *) file.data;
        file.data += sizeof(byte);
    }
    return hash_result;
}

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

static void
__add_pathname_and_version_to_list(const char *restrict pathname, list *list) {
    versioned_pathname *vpathname = __MALLOC__(1, versioned_pathname);
    memcpy(vpathname->pathname, pathname, MAX_PATHNAME_SIZE);
    entire_file file = read_entire_file_into_memory(pathname);
    if (IS_EMPTY(file)) {
        free(vpathname);
        return;
    }
    vpathname->version = compute_file_hash(file);
    list_rpush(list, vpathname);
}

void get_all_pathnames_and_versions(const char *restrict root_directory, list *pathnames_out) {
    char path_buffer[1024];
    struct dirent *entry;
    DIR *dir = opendir(root_directory);

    if (!dir) return;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            strcpy(path_buffer, root_directory);
            strcat(path_buffer, "/");
            strcat(path_buffer, entry->d_name);

            if (entry->d_type == DT_REG) {
                __add_pathname_and_version_to_list(path_buffer, pathnames_out);
            } else {
                get_all_pathnames_and_versions(path_buffer, pathnames_out);
            }
        }
    }

    closedir(dir);
}