#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>

typedef struct {
  const char* szKey;
  const char* szValue;
} ConfigDefault;

static const ConfigDefault rgConfigDefaults[] = {
  {"natal.date", "1972-04-08"},
  {"natal.time", "0:00"},
  {"natal.tz", "-12"},
  {"natal.location", "176:14E 38:08S"}
};

static const size_t cConfigDefaults = sizeof(rgConfigDefaults) / sizeof(rgConfigDefaults[0]);

const char* CONFIG_DIR_REL = "/.config/astro";
const char* CONFIG_FILE_NAME = "config.ini";

static int ConfigGetDefaultValue(const char* szKey, char* value_buffer, size_t buffer_size) {
  for (size_t i = 0; i < cConfigDefaults; i++) {
    if (strcmp(rgConfigDefaults[i].szKey, szKey) == 0) {
      snprintf(value_buffer, buffer_size, "%s", rgConfigDefaults[i].szValue);
      return 0;
    }
  }
  return -1;
}

int get_config_path(char* path_buffer, size_t buffer_size) {
    const char* home_dir = getenv("HOME");
    if (!home_dir) {
        fprintf(stderr, "Error: HOME environment variable not set.\n");
        return -1;
    }
    snprintf(path_buffer, buffer_size, "%s%s/%s", home_dir, CONFIG_DIR_REL, CONFIG_FILE_NAME);
    return 0;
}

int ensure_config_path_exists() {
    char dir_path[MAX_PATH_LEN];
    const char* home_dir = getenv("HOME");
    if (!home_dir) {
        fprintf(stderr, "Error: HOME environment variable not set.\n");
        return -1;
    }
    snprintf(dir_path, MAX_PATH_LEN, "%s%s", home_dir, CONFIG_DIR_REL);

    // The 'mkdir' command with '-p' is a convenient way to create parent directories.
    // A more robust C-native way would be to create each directory level one by one.
    char command[MAX_PATH_LEN + 10];
    snprintf(command, sizeof(command), "mkdir -p %s", dir_path);
    
    int result = system(command);
    if (result != 0) {
        // Check if the directory already exists, which system() might report as an error.
        struct stat st;
        if (stat(dir_path, &st) == 0 && S_ISDIR(st.st_mode)) {
            return 0; // Directory exists, which is fine.
        }
        fprintf(stderr, "Warning: Failed to create directory %s\n", dir_path);
        // We don't return -1 here, as the file open might still succeed.
    }
    return 0;
}

int config_get_value(const char* key, char* value_buffer, size_t buffer_size) {
    char config_path[MAX_PATH_LEN];
    if (get_config_path(config_path, sizeof(config_path)) != 0) {
        return ConfigGetDefaultValue(key, value_buffer, buffer_size);
    }

    FILE* file = fopen(config_path, "r");
    if (!file) {
        // This is not an error if the file just doesn't exist yet.
        return ConfigGetDefaultValue(key, value_buffer, buffer_size);
    }

    char line[MAX_CONFIG_LINE_LEN];
    char current_key[MAX_KEY_LEN];
    int found = -1;

    while (fgets(line, sizeof(line), file)) {
        // Find the position of '='
        char* separator = strchr(line, '=');
        if (!separator) {
            continue; // Not a valid key-value line
        }

        // Extract key
        size_t key_len = separator - line;
        if (key_len < sizeof(current_key)) {
            strncpy(current_key, line, key_len);
            current_key[key_len] = '\0';

            if (strcmp(current_key, key) == 0) {
                // Key found, extract value
                char* value_start = separator + 1;
                // Remove newline character if present
                char* newline = strchr(value_start, '\n');
                if (newline) {
                    *newline = '\0';
                }
                strncpy(value_buffer, value_start, buffer_size - 1);
                value_buffer[buffer_size - 1] = '\0';
                found = 0;
                break;
            }
        }
    }

    fclose(file);
    if (found == 0) {
        return 0;
    }
    return ConfigGetDefaultValue(key, value_buffer, buffer_size);
}

// A simple struct to hold key-value pairs from the config file
typedef struct {
    char key[MAX_KEY_LEN];
    char value[MAX_VALUE_LEN];
} ConfigEntry;

int config_set_value(const char* key, const char* value) {
    if (ensure_config_path_exists() != 0) {
        return -1;
    }
    char config_path[MAX_PATH_LEN];
    if (get_config_path(config_path, sizeof(config_path)) != 0) {
        return -1;
    }

    // This implementation reads all entries, updates/adds the new one, and rewrites the file.
    ConfigEntry entries[100]; // Max 100 entries for simplicity
    int entry_count = 0;
    int key_found = 0;

    FILE* file = fopen(config_path, "r");
    if (file) {
        char line[MAX_CONFIG_LINE_LEN];
        while (fgets(line, sizeof(line), file) && entry_count < 100) {
            char* separator = strchr(line, '=');
            if (separator) {
                size_t key_len = separator - line;
                if (key_len < MAX_KEY_LEN) {
                    strncpy(entries[entry_count].key, line, key_len);
                    entries[entry_count].key[key_len] = '\0';

                    char* value_start = separator + 1;
                    char* newline = strchr(value_start, '\n');
                    if (newline) *newline = '\0';

                    strncpy(entries[entry_count].value, value_start, MAX_VALUE_LEN - 1);
                    entries[entry_count].value[MAX_VALUE_LEN - 1] = '\0';

                    if (strcmp(entries[entry_count].key, key) == 0) {
                        // Update existing key
                        strncpy(entries[entry_count].value, value, MAX_VALUE_LEN - 1);
                        key_found = 1;
                    }
                    entry_count++;
                }
            }
        }
        fclose(file);
    }
    
    // If key was not found, add it as a new entry
    if (!key_found && entry_count < 100) {
        strncpy(entries[entry_count].key, key, MAX_KEY_LEN - 1);
        entries[entry_count].key[MAX_KEY_LEN - 1] = '\0';
        strncpy(entries[entry_count].value, value, MAX_VALUE_LEN - 1);
        entries[entry_count].value[MAX_VALUE_LEN - 1] = '\0';
        entry_count++;
    }

    // Rewrite the file
    file = fopen(config_path, "w");
    if (!file) {
        perror("fopen for writing failed");
        return -1;
    }

    for (int i = 0; i < entry_count; i++) {
        fprintf(file, "%s=%s\n", entries[i].key, entries[i].value);
    }

    fclose(file);
    return 0;
}
