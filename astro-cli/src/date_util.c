#include "date_util.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h> // For sscanf

// Helper array for days in each month (non-leap year)
static const int days_in_month[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

// Checks if a year is a leap year
static int is_leap_year(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

// Gets the number of days in a given month of a given year
static int get_days_in_month(int year, int month) {
    if (month < 1 || month > 12) return 0;
    if (month == 2 && is_leap_year(year)) return 29;
    return days_in_month[month];
}

int parse_date_string(const char* date_str, Date* date) {
    if (!date_str || !date) return -1;
    if (sscanf(date_str, "%d-%d-%d", &date->year, &date->month, &date->day) == 3) {
        if (date->year < 0 || date->month < 1 || date->month > 12 || date->day < 1 || date->day > get_days_in_month(date->year, date->month)) {
            return -1; // Invalid date components
        }
        return 0;
    }
    return -1; // Parsing failed
}

int date_to_string(const Date* date, char* str, size_t size) {
    if (!date || !str) return -1;
    int written = snprintf(str, size, "%04d-%02d-%02d", date->year, date->month, date->day);
    if (written >= size || written < 0) {
        return -1; // Buffer too small or error
    }
    return 0;
}

void increment_date(Date* date) {
    date->day++;
    int days_in_current_month = get_days_in_month(date->year, date->month);

    if (date->day > days_in_current_month) {
        date->day = 1;
        date->month++;
        if (date->month > 12) {
            date->month = 1;
            date->year++;
        }
    }
}

int compare_dates(const Date* date1, const Date* date2) {
    if (date1->year != date2->year) {
        return date1->year - date2->year;
    }
    if (date1->month != date2->month) {
        return date1->month - date2->month;
    }
    return date1->day - date2->day;
}
