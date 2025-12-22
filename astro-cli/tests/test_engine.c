#include <stdlib.h>
#include <string.h>
#include "minunit.h"
#include "../src/engine.h"

int tests_run = 0;

static char* test_run_astrolog_command_uses_stub() {
    const char *prev = getenv("ASTROLOG_BIN");
    setenv("ASTROLOG_BIN", "./tests/fake_astrolog.sh", 1);

    char buffer[256];
    int rc = run_astrolog_command("-qa 1 1 2000", buffer, sizeof(buffer));

    if (prev) {
        setenv("ASTROLOG_BIN", prev, 1);
    } else {
        unsetenv("ASTROLOG_BIN");
    }

    mu_assert("run_astrolog_command failed", rc == 0);
    mu_assert("stub output missing", strstr(buffer, "stub astrolog args: -qa 1 1 2000") != NULL);

    return 0;
}

static char* all_tests() {
    mu_run_test(test_run_astrolog_command_uses_stub);
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
