#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>

#define MAX_PATH_LEN 1024
#define MAX_CONFIG_LINE_LEN 256
#define MAX_KEY_LEN 64
#define MAX_VALUE_LEN 128

// Gets the full path to the configuration file.
// Returns 0 on success.
int get_config_path(char* path_buffer, size_t buffer_size);

// Ensures the directory for the config file exists.
// Returns 0 on success.
int ensure_config_path_exists();

// Retrieves a value for a given key from the config file.
// Returns 0 on success (key found), -1 on failure (key not found or error).
int config_get_value(const char* key, char* value_buffer, size_t buffer_size);

// Sets a value for a given key in the config file.
// This will rewrite the entire config file.
// Returns 0 on success, -1 on failure.
int config_set_value(const char* key, const char* value);

#endif // CONFIG_H
