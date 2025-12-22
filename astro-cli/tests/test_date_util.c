#include <stdio.h>
#include <string.h>
#include "minunit.h"
#include "../src/date_util.h"

int tests_run = 0;

static char* test_parse_date_string() {
    Date d;
    mu_assert("parse_date_string failed for valid date", parse_date_string("2023-10-27", &d) == 0);
    mu_assert("year wrong", d.year == 2023);
    mu_assert("month wrong", d.month == 10);
    mu_assert("day wrong", d.day == 27);
    
    mu_assert("parse_date_string should fail for invalid format", parse_date_string("invalid", &d) != 0);
    mu_assert("parse_date_string should fail for invalid month", parse_date_string("2023-13-01", &d) != 0);
    mu_assert("parse_date_string should fail for invalid day", parse_date_string("2023-02-30", &d) != 0);
    
    return 0;
}

static char* test_date_to_string() {
    Date d = {2023, 10, 27};
    char buf[16];
    mu_assert("date_to_string failed", date_to_string(&d, buf, sizeof(buf)) == 0);
    mu_assert("date string format wrong", strcmp(buf, "2023-10-27") == 0);
    
    return 0;
}

static char* test_increment_date() {
    Date d = {2023, 10, 31};
    increment_date(&d);
    mu_assert("increment from month end failed", d.day == 1 && d.month == 11 && d.year == 2023);
    
    d.year = 2023; d.month = 12; d.day = 31;
    increment_date(&d);
    mu_assert("increment from year end failed", d.day == 1 && d.month == 1 && d.year == 2024);
    
    d.year = 2024; d.month = 2; d.day = 28; // Leap year
    increment_date(&d);
    mu_assert("increment leap day failed", d.day == 29 && d.month == 2);
    increment_date(&d);
    mu_assert("increment after leap day failed", d.day == 1 && d.month == 3);
    
    return 0;
}

static char* test_compare_dates() {
    Date d1 = {2023, 10, 27};
    Date d2 = {2023, 10, 28};
    Date d3 = {2023, 10, 27};
    Date d4 = {2022, 12, 31};
    
    mu_assert("compare_dates less than failed", compare_dates(&d1, &d2) < 0);
    mu_assert("compare_dates equal failed", compare_dates(&d1, &d3) == 0);
    mu_assert("compare_dates greater than failed", compare_dates(&d2, &d1) > 0);
    mu_assert("compare_dates year check failed", compare_dates(&d1, &d4) > 0);
    
    return 0;
}

static char* all_tests() {
    mu_run_test(test_parse_date_string);
    mu_run_test(test_date_to_string);
    mu_run_test(test_increment_date);
    mu_run_test(test_compare_dates);
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
