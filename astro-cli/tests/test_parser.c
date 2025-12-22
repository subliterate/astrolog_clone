#include <stdio.h>
#include <string.h>
#include "minunit.h"
#include "../src/parser.h"

int tests_run = 0;

static char* test_parse_v_output_parses_planets() {
    const char *sample =
        "Body  Locat.\n"
        "Sun :  5Ari20 R [02]\n"
        "Moon: 17Lib05   [07]\n"
        "Car Fix Mut TOT\n";

    PlanetInfo planets[MAX_PLANETS];
    int count = parse_v_output(sample, planets, MAX_PLANETS);

    mu_assert("expected two planets", count == 2);

    mu_assert("Sun name wrong", strcmp(planets[0].name, "Sun") == 0);
    mu_assert("Sun sign wrong", strcmp(planets[0].sign, "Ari") == 0);
    mu_assert("Sun degrees wrong", planets[0].degrees == 5);
    mu_assert("Sun minutes wrong", planets[0].minutes == 20);
    mu_assert("Sun retrograde flag wrong", planets[0].is_retrograde == 'R');
    mu_assert("Sun house wrong", planets[0].house == 2);

    mu_assert("Moon name wrong", strcmp(planets[1].name, "Moon") == 0);
    mu_assert("Moon sign wrong", strcmp(planets[1].sign, "Lib") == 0);
    mu_assert("Moon degrees wrong", planets[1].degrees == 17);
    mu_assert("Moon minutes wrong", planets[1].minutes == 5);
    mu_assert("Moon retrograde flag wrong", planets[1].is_retrograde == ' ');
    mu_assert("Moon house wrong", planets[1].house == 7);

    return 0;
}

static char* test_parse_aspect_output_parses_lines() {
    const char *sample =
        "  1: Sun (Ari) Con Moon (Cap)\n"
        "  2: Mars (Gem) Opp Jupiter (Sag)\n"
        "Sum power:\n";

    AspectInfo aspects[MAX_PLANETS];
    int count = parse_aspect_output(sample, aspects, MAX_PLANETS);

    mu_assert("expected two aspects", count == 2);

    mu_assert("aspect 1 body1 wrong", strcmp(aspects[0].body1, "Sun") == 0);
    mu_assert("aspect 1 aspect wrong", strcmp(aspects[0].aspect, "Con") == 0);
    mu_assert("aspect 1 body2 wrong", strcmp(aspects[0].body2, "Moon") == 0);

    mu_assert("aspect 2 body1 wrong", strcmp(aspects[1].body1, "Mars") == 0);
    mu_assert("aspect 2 aspect wrong", strcmp(aspects[1].aspect, "Opp") == 0);
    mu_assert("aspect 2 body2 wrong", strcmp(aspects[1].body2, "Jupiter") == 0);

    return 0;
}

static char* all_tests() {
    mu_run_test(test_parse_v_output_parses_planets);
    mu_run_test(test_parse_aspect_output_parses_lines);
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
