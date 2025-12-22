#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "minunit.h"
#include "../src/config.h"

int tests_run = 0;

static int set_temp_home(char *buffer, size_t size) {
    const char template[] = "/tmp/astro_cli_config.XXXXXX";
    if (size < sizeof(template)) {
        return -1;
    }

    strncpy(buffer, template, size - 1);
    buffer[size - 1] = '\0';

    char *dir = mkdtemp(buffer);
    if (!dir) {
        return -1;
    }

    if (setenv("HOME", dir, 1) != 0) {
        return -1;
    }

    return 0;
}

static void cleanup_home(const char *path) {
    char command[256];
    snprintf(command, sizeof(command), "rm -rf %s", path);
    system(command);
}

static char* test_set_and_get_value() {
    char temp_home[MAX_PATH_LEN];
    mu_assert("failed to create temp HOME", set_temp_home(temp_home, sizeof(temp_home)) == 0);

    mu_assert("ensure_config_path_exists failed", ensure_config_path_exists() == 0);
    mu_assert("config_set_value failed", config_set_value("natal.date", "2000-01-01") == 0);

    char value[MAX_VALUE_LEN];
    mu_assert("config_get_value failed", config_get_value("natal.date", value, sizeof(value)) == 0);
    mu_assert("config_get_value returned wrong value", strcmp(value, "2000-01-01") == 0);

    cleanup_home(temp_home);
    return 0;
}

static char* test_updates_existing_key() {
    char temp_home[MAX_PATH_LEN];
    mu_assert("failed to create temp HOME", set_temp_home(temp_home, sizeof(temp_home)) == 0);

    mu_assert("initial set failed", config_set_value("natal.tz", "-5") == 0);
    mu_assert("update set failed", config_set_value("natal.tz", "2") == 0);

    char value[MAX_VALUE_LEN];
    mu_assert("config_get_value failed after update", config_get_value("natal.tz", value, sizeof(value)) == 0);
    mu_assert("updated value incorrect", strcmp(value, "2") == 0);

    cleanup_home(temp_home);
    return 0;
}

static char* test_missing_key_returns_error() {
    char temp_home[MAX_PATH_LEN];
    mu_assert("failed to create temp HOME", set_temp_home(temp_home, sizeof(temp_home)) == 0);

    char value[MAX_VALUE_LEN];
    mu_assert("expected missing key to return error", config_get_value("does.not.exist", value, sizeof(value)) != 0);

    cleanup_home(temp_home);
    return 0;
}

static char* all_tests() {
    mu_run_test(test_set_and_get_value);
    mu_run_test(test_updates_existing_key);
    mu_run_test(test_missing_key_returns_error);
    return 0;
}

int main(int argc, char **argv) {
    char *result = all_tests();
    if (result != 0) {
        printf("TEST FAILED: %s\n", result);
    } else {
        printf("ALL TESTS PASSED\n");
    }
    printf("Tests run: %d\n", tests_run);

    return result != 0;
}
