#include "engine.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#define MAX_COMMAND_LEN 2048

#include <stdlib.h>

static int path_is_executable(const char* path) {
    return access(path, X_OK) == 0;
}

const char* get_astrolog_executable() {
    const char* env_path = getenv("ASTROLOG_BIN");
    if (env_path && env_path[0] != '\0') {
        return env_path;
    }

    static char local_path[PATH_MAX];
    if (path_is_executable("./astrolog")) {
        snprintf(local_path, sizeof(local_path), "./astrolog");
        return local_path;
    }

    if (path_is_executable("../astrolog")) {
        snprintf(local_path, sizeof(local_path), "../astrolog");
        return local_path;
    }

#ifdef __linux__
    char exe_path[PATH_MAX];
    ssize_t exe_len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
    if (exe_len > 0) {
        exe_path[exe_len] = '\0';
        char* last_slash = strrchr(exe_path, '/');
        if (last_slash) {
            *last_slash = '\0';
            const char* suffix_primary = "/../../astrolog";
            const char* suffix_fallback = "/../astrolog";
            size_t base_len = strlen(exe_path);
            size_t suffix_len = strlen(suffix_primary);
            if (base_len + suffix_len < sizeof(local_path)) {
                memcpy(local_path, exe_path, base_len);
                memcpy(local_path + base_len, suffix_primary, suffix_len + 1);
                if (path_is_executable(local_path)) {
                    return local_path;
                }
            }
            suffix_len = strlen(suffix_fallback);
            if (base_len + suffix_len < sizeof(local_path)) {
                memcpy(local_path, exe_path, base_len);
                memcpy(local_path + base_len, suffix_fallback, suffix_len + 1);
                if (path_is_executable(local_path)) {
                    return local_path;
                }
            }
        }
    }
#endif

    return "astrolog"; // Fall back to PATH lookup
}

int run_astrolog_command(const char* args, char* output_buffer, size_t buffer_size) {
    char command[MAX_COMMAND_LEN];
    FILE *pipe;
    const char* executable = get_astrolog_executable();

    // Clear the output buffer
    output_buffer[0] = '\0';

    // Construct the full command
    int written = snprintf(command, MAX_COMMAND_LEN, "%s %s", executable, args);
    if (written >= MAX_COMMAND_LEN) {
        fprintf(stderr, "Error: Command arguments are too long.\n");
        return -1;
    }

    // Open a pipe to the astrolog executable
    pipe = popen(command, "r");
    if (!pipe) {
        perror("popen failed");
        fprintf(stderr, "Error: Failed to execute command: %s\n", command);
        return -1;
    }

    // Read the output from the pipe
    size_t total_read = 0;
    char* current_pos = output_buffer;
    while (total_read < buffer_size - 1 && fgets(current_pos, buffer_size - total_read, pipe)) {
        size_t len = strlen(current_pos);
        total_read += len;
        current_pos += len;
    }
    
    // Check for read errors or if the buffer was filled
    if (ferror(pipe)) {
        perror("fgets failed");
        pclose(pipe);
        return -1;
    }

    if (!feof(pipe)) {
        fprintf(stderr, "Warning: Output buffer may be too small. Output was truncated.\n");
    }

    // Close the pipe
    int status = pclose(pipe);
    if (status == -1) {
        perror("pclose failed");
        return -1;
    }
    
    // popen does not give us the exit code of the child process directly
    // in a simple way, but for now, we'll consider a successful read as success.

    return 0;
}
