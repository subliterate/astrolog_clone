#ifndef DATE_UTIL_H
#define DATE_UTIL_H

#include <stddef.h> // For size_t

typedef struct {
    int year;
    int month;
    int day;
} Date;

// Parses a date string in YYYY-MM-DD format into a Date struct.
// Returns 0 on success, -1 on failure.
int parse_date_string(const char* date_str, Date* date);

// Formats a Date struct into a string in YYYY-MM-DD format.
// Returns 0 on success, -1 on failure.
int date_to_string(const Date* date, char* str, size_t size);

// Increments the given Date struct by one day.
void increment_date(Date* date);

// Compares two Date structs.
// Returns: < 0 if date1 is earlier, 0 if dates are equal, > 0 if date1 is later.
int compare_dates(const Date* date1, const Date* date2);

#endif // DATE_UTIL_H
