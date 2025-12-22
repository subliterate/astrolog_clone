#ifndef ENGINE_H
#define ENGINE_H

#include <stdio.h>

// Executes a command on the original astrolog executable and captures the output.
// args: The command-line arguments to pass to the astrolog executable.
// output_buffer: The buffer to store the captured output.
// buffer_size: The size of the output buffer.
// Returns: 0 on success, -1 on failure.
int run_astrolog_command(const char* args, char* output_buffer, size_t buffer_size);

#endif // ENGINE_H
