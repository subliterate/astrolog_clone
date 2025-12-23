#include "command.h"
#include "engine.h"
#include "config.h"
#include "parser.h"
#include "date_util.h" // Include the new date utility
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define OUTPUT_BUFFER_SIZE 8192
#define ASTROLOG_ARGS_MAX 2048
#define MAX_LIST_ITEMS 32
#define MAX_ASPECTS 16

typedef struct {
    const char* szCode;
    const char* szName;
} AspectDef;

static const AspectDef rgAspects[] = {
    {"Con", "Conjunction"},
    {"Opp", "Opposition"},
    {"Squ", "Square"},
    {"Tri", "Trine"},
    {"Sex", "Sextile"}
};

static const int cAspects = sizeof(rgAspects) / sizeof(rgAspects[0]);

// A simple helper to find an option (e.g., --date) and return its value.
// Returns the value, or NULL if not found.
const char* get_option(int argc, char* argv[], const char* option_name) {
    for (int i = 0; i < argc - 1; i++) {
        if (strcmp(argv[i], option_name) == 0) {
            return argv[i + 1];
        }
    }
    return NULL;
}

static int has_flag(int argc, char* argv[], const char* flag_name) {
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], flag_name) == 0) {
            return 1;
        }
    }
    return 0;
}

static int strings_equal_ignore_case(const char* a, const char* b) {
    if (!a || !b) return 0;
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) {
            return 0;
        }
        a++;
        b++;
    }
    return *a == '\0' && *b == '\0';
}

static int starts_with_ignore_case(const char* str, const char* prefix) {
    if (!str || !prefix) return 0;
    while (*prefix) {
        if (*str == '\0') return 0;
        if (tolower((unsigned char)*str) != tolower((unsigned char)*prefix)) {
            return 0;
        }
        str++;
        prefix++;
    }
    return 1;
}

static char* trim_whitespace(char* str) {
    char* start = str;
    char* end;
    if (!str) return str;
    while (*start && isspace((unsigned char)*start)) start++;
    end = start + strlen(start);
    while (end > start && isspace((unsigned char)*(end - 1))) end--;
    *end = '\0';
    return start;
}

static int parse_csv_list(const char* input, char items[][MAX_NAME_LEN], int max_items) {
    if (!input || !items || max_items <= 0) return 0;
    char buffer[MAX_VALUE_LEN];
    int count = 0;

    snprintf(buffer, sizeof(buffer), "%s", input);
    char* token = strtok(buffer, ",");
    while (token && count < max_items) {
        char* trimmed = trim_whitespace(token);
        if (*trimmed != '\0') {
            snprintf(items[count], MAX_NAME_LEN, "%s", trimmed);
            count++;
        }
        token = strtok(NULL, ",");
    }
    return count;
}

static int load_all_aspects(char aspects[][MAX_NAME_LEN], int max_aspects) {
    int count = 0;
    for (int i = 0; i < cAspects && count < max_aspects; i++) {
        snprintf(aspects[count], MAX_NAME_LEN, "%s", rgAspects[i].szCode);
        count++;
    }
    return count;
}

static int normalize_aspect_token(const char* token, char* out, size_t out_size) {
    if (!token || !out || out_size == 0) return -1;
    if (strings_equal_ignore_case(token, "all") || strings_equal_ignore_case(token, "any")) {
        return -2;
    }

    char* endptr = NULL;
    long index = strtol(token, &endptr, 10);
    if (endptr && *endptr == '\0') {
        if (index < 1 || index > cAspects) {
            return -1;
        }
        snprintf(out, out_size, "%s", rgAspects[index - 1].szCode);
        return 0;
    }

    for (int i = 0; i < cAspects; i++) {
        if (strings_equal_ignore_case(token, rgAspects[i].szCode) ||
            strings_equal_ignore_case(token, rgAspects[i].szName)) {
            snprintf(out, out_size, "%s", rgAspects[i].szCode);
            return 0;
        }
    }

    return -1;
}

static int parse_aspect_list(const char* input, char aspects[][MAX_NAME_LEN], int max_aspects) {
    if (!input || !aspects || max_aspects <= 0) return 0;

    char buffer[MAX_VALUE_LEN];
    int count = 0;
    snprintf(buffer, sizeof(buffer), "%s", input);

    char* token = strtok(buffer, ",");
    while (token && count < max_aspects) {
        char* trimmed = trim_whitespace(token);
        if (*trimmed == '\0') {
            token = strtok(NULL, ",");
            continue;
        }

        char normalized[MAX_NAME_LEN];
        int result = normalize_aspect_token(trimmed, normalized, sizeof(normalized));
        if (result == -2) {
            return load_all_aspects(aspects, max_aspects);
        }
        if (result != 0) {
            return -1;
        }

        int already_present = 0;
        for (int i = 0; i < count; i++) {
            if (strings_equal_ignore_case(aspects[i], normalized)) {
                already_present = 1;
                break;
            }
        }
        if (!already_present) {
            snprintf(aspects[count], MAX_NAME_LEN, "%s", normalized);
            count++;
        }

        token = strtok(NULL, ",");
    }

    return count;
}

static int name_matches(const char* candidate, const char* requested) {
    size_t requested_len;
    if (!candidate || !requested) return 0;
    if (strings_equal_ignore_case(candidate, requested)) return 1;
    requested_len = strlen(requested);
    if (requested_len >= 3 && starts_with_ignore_case(candidate, requested)) {
        return 1;
    }
    return 0;
}

static int name_in_list(const char* candidate, char items[][MAX_NAME_LEN], int count) {
    for (int i = 0; i < count; i++) {
        if (name_matches(candidate, items[i])) {
            return 1;
        }
    }
    return 0;
}

static int aspect_in_list(const char* candidate, char aspects[][MAX_NAME_LEN], int count) {
    for (int i = 0; i < count; i++) {
        if (strings_equal_ignore_case(candidate, aspects[i])) {
            return 1;
        }
    }
    return 0;
}

static void print_aspect_list() {
    printf("Aspects (index: code - name):\n");
    for (int i = 0; i < cAspects; i++) {
        printf("  %d: %s - %s\n", i + 1, rgAspects[i].szCode, rgAspects[i].szName);
    }
}

void print_data_usage() {
    printf("Usage: astro data <subcommand> [options]\n\n");
    printf("Subcommand:\n");
    printf("  positions      Get the positions of celestial bodies.\n\n");
    printf("Options for positions:\n");
    printf("  --date <YYYY-MM-DD>   Date for the chart. Defaults to natal.date config.\n");
    printf("  --time <HH:MM>        Time for the chart. Defaults to natal.time config.\n");
    printf("  --tz <tz>             Timezone. Defaults to natal.tz config.\n");
    printf("  --loc <lon lat>       Location. Defaults to natal.location config.\n");
    printf("  --output <format>     Output format (e.g., 'csv'). Defaults to text.\n");
}


int handle_data_command(int argc, char *argv[]) {
    if (argc < 2 || strcmp(argv[1], "positions") != 0) {
        print_data_usage();
        return 1;
    }
    
    // Default chart data - we'll try to override these with args or config
    char date_str[MAX_VALUE_LEN] = "1972-04-08";
    char time_str[MAX_VALUE_LEN] = "0:00";
    char tz_str[MAX_VALUE_LEN] = "-12";
    char loc_str[MAX_VALUE_LEN] = "176:14E 38:08S";
    char lon_str[MAX_VALUE_LEN]; // New
    char lat_str[MAX_VALUE_LEN]; // New

    // Try to get values from config first
    config_get_value("natal.date", date_str, sizeof(date_str));
    config_get_value("natal.time", time_str, sizeof(time_str));
    config_get_value("natal.tz", tz_str, sizeof(tz_str));
    config_get_value("natal.location", loc_str, sizeof(loc_str));
    
    // Override with command-line options if they exist
    const char* opt_date = get_option(argc, argv, "--date");
    const char* opt_time = get_option(argc, argv, "--time");
    const char* opt_tz = get_option(argc, argv, "--tz");
    const char* opt_loc = get_option(argc, argv, "--loc");
    const char* opt_output = get_option(argc, argv, "--output");

    if (opt_date) strncpy(date_str, opt_date, sizeof(date_str) - 1);
    if (opt_time) strncpy(time_str, opt_time, sizeof(time_str) - 1);
    if (opt_tz) strncpy(tz_str, opt_tz, sizeof(tz_str) - 1);
    if (opt_loc) strncpy(loc_str, opt_loc, sizeof(loc_str) - 1);

    // Parse loc_str into lon and lat
    if (sscanf(loc_str, "%s %s", lon_str, lat_str) != 2) {
        fprintf(stderr, "Error: Invalid location format. Expected \"LON LAT\".\n");
        return 1;
    }

    // Now construct the astrolog args
  char astrolog_args[ASTROLOG_ARGS_MAX];
    char final_date_str[MAX_VALUE_LEN];

    int year, month, day;
    // Try to parse the date as YYYY-MM-DD
    if (sscanf(date_str, "%d-%d-%d", &year, &month, &day) == 3) {
        snprintf(final_date_str, sizeof(final_date_str), "%d %d %d", month, day, year);
    } else {
        // Fallback for MM-DD-YYYY or other formats, just replace hyphens
        strncpy(final_date_str, date_str, sizeof(final_date_str));
        for (int i = 0; final_date_str[i]; i++) {
            if (final_date_str[i] == '-') final_date_str[i] = ' ';
        }
    }
    
    snprintf(astrolog_args, sizeof(astrolog_args), "-v -qa %s %s %s %s %s",
        final_date_str, time_str, tz_str, lon_str, lat_str);

    char output[OUTPUT_BUFFER_SIZE];
    
    if (run_astrolog_command(astrolog_args, output, OUTPUT_BUFFER_SIZE) != 0) {
        fprintf(stderr, "Error: Failed to execute and capture astrolog command.\n");
        return 1;
    }

    if (opt_output && strcmp(opt_output, "csv") == 0) {
        PlanetInfo planets[MAX_PLANETS];
        int planet_count = parse_v_output(output, planets, MAX_PLANETS);
        if (planet_count > 0) {
            output_csv(planets, planet_count);
        } else {
            fprintf(stderr, "Error: Failed to parse astrolog output.\n");
            return 1;
        }
    } else {
        // Default text output
        printf("Executing astrolog command with args: %s\n\n", astrolog_args);
        printf("%s", output);
    }

    return 0;
}

void print_config_usage() {
    printf("Usage: astro config <subcommand> [key] [value]\n\n");
    printf("Subcommands:\n");
    printf("  get <key>         Get the value for a given configuration key.\n");
    printf("  set <key> <value>   Set the value for a given configuration key.\n\n");
    printf("Example:\n");
    printf("  astro config set natal.date \"1990-01-01\"\n");
    printf("  astro config get natal.date\n");
}

int handle_config_command(int argc, char *argv[]) {
    if (argc < 2) {
        print_config_usage();
        return 1;
    }
    char* subcommand = argv[1];
    if (strcmp(subcommand, "get") == 0) {
        if (argc != 3) {
            print_config_usage();
            return 1;
        }
        char* key = argv[2];
        char value[MAX_VALUE_LEN];
        if (config_get_value(key, value, sizeof(value)) == 0) {
            printf("%s\n", value);
        } else {
            fprintf(stderr, "Error: Key '%s' not found.\n", key);
            return 1;
        }
    } else if (strcmp(subcommand, "set") == 0) {
        if (argc != 4) {
            print_config_usage();
            return 1;
        }
        char* key = argv[2];
        char* value = argv[3];
        if (config_set_value(key, value) == 0) {
            printf("Set %s = %s\n", key, value);
        } else {
            fprintf(stderr, "Error: Failed to set key '%s'.\n", key);
            return 1;
        }
    } else {
        printf("Unknown config subcommand: %s\n\n", subcommand);
        print_config_usage();
        return 1;
    }
    return 0;
}

void print_chart_usage() {
    printf("Usage: astro chart <subcommand> [options]\n\n");
    printf("Subcommand:\n");
    printf("  natal          Generate a natal (birth) chart.\n\n");
    printf("Options for natal:\n");
    printf("  --date <YYYY-MM-DD>   Date for the chart. Defaults to natal.date config.\n");
    printf("  --time <HH:MM>        Time for the chart. Defaults to natal.time config.\n");
    printf("  --tz <tz>             Timezone. Defaults to natal.tz config.\n");
    printf("  --loc <lon lat>       Location. Defaults to natal.location config.\n");
}

int handle_chart_command(int argc, char *argv[]) {
    if (argc < 2 || strcmp(argv[1], "natal") != 0) {
        print_chart_usage();
        return 1;
    }

    // Default chart data - we'll try to override these with args or config
    char date_str[MAX_VALUE_LEN]; strcpy(date_str, "1972-04-08");
    char time_str[MAX_VALUE_LEN]; strcpy(time_str, "0:00");
    char tz_str[MAX_VALUE_LEN]; strcpy(tz_str, "-12");
    char loc_str[MAX_VALUE_LEN]; strcpy(loc_str, "176:14E 38:08S");
    char lon_str[MAX_VALUE_LEN];
    char lat_str[MAX_VALUE_LEN];

    // Try to get values from config first
    config_get_value("natal.date", date_str, sizeof(date_str));
    config_get_value("natal.time", time_str, sizeof(time_str));
    config_get_value("natal.tz", tz_str, sizeof(tz_str));
    config_get_value("natal.location", loc_str, sizeof(loc_str));
    
    // Override with command-line options if they exist
    const char* opt_date = get_option(argc, argv, "--date");
    const char* opt_time = get_option(argc, argv, "--time");
    const char* opt_tz = get_option(argc, argv, "--tz");
    const char* opt_loc = get_option(argc, argv, "--loc");

    if (opt_date) strncpy(date_str, opt_date, sizeof(date_str) - 1);
    if (opt_time) strncpy(time_str, opt_time, sizeof(time_str) - 1);
    if (opt_tz) strncpy(tz_str, opt_tz, sizeof(tz_str) - 1);
    if (opt_loc) strncpy(loc_str, opt_loc, sizeof(loc_str) - 1);

    // Parse loc_str into lon and lat
    if (sscanf(loc_str, "%s %s", lon_str, lat_str) != 2) {
        fprintf(stderr, "Error: Invalid location format. Expected \"LON LAT\".\n");
        return 1;
    }

    // Now construct the astrolog args
  char astrolog_args[ASTROLOG_ARGS_MAX];
    char final_date_str[MAX_VALUE_LEN];

    int year, month, day;
    // Try to parse the date as YYYY-MM-DD
    if (sscanf(date_str, "%d-%d-%d", &year, &month, &day) == 3) {
        snprintf(final_date_str, sizeof(final_date_str), "%d %d %d", month, day, year);
    } else {
        // Fallback for other formats, just replace hyphens (less robust)
        strncpy(final_date_str, date_str, sizeof(final_date_str));
        for (int i = 0; final_date_str[i]; i++) {
            if (final_date_str[i] == '-') final_date_str[i] = ' ';
        }
    }
    
    // Using -w for a wheel chart, -qa for quick chart entry
    snprintf(astrolog_args, sizeof(astrolog_args), "-w -qa %s %s %s %s %s",
        final_date_str, time_str, tz_str, lon_str, lat_str);

    char output[OUTPUT_BUFFER_SIZE];
    
    if (run_astrolog_command(astrolog_args, output, OUTPUT_BUFFER_SIZE) != 0) {
        fprintf(stderr, "Error: Failed to execute and capture astrolog command.\n");
        return 1;
    }

    // For now, no specific parsing/formatting for chart commands, just print raw output
    printf("Executing astrolog command with args: %s\n\n", astrolog_args);
    printf("%s", output);

    return 0;
}

void print_predict_usage() {
    printf("Usage: astro predict <subcommand> [options]\n\n");
    printf("Subcommand:\n");
    printf("  transits       Find transit events over a date range.\n");
    printf("  returns        Find planetary return events.\n");
    printf("  stations       Find planetary station (retrograde/direct) events.\n\n");
    printf("Options for transits:\n");
    printf("  --start <YYYY-MM-DD>  Start date for the search. Default: today.\n");
    printf("  --end <YYYY-MM-DD>    End date for the search. Default: start + 30 days.\n");
    printf("  --days <n>            Range length in days when --end is omitted. Default: 30.\n");
    printf("  --body <name(s)>      Transiting body list (comma-separated). Default: Sun.\n");
    printf("  --target-body <name>  Natal body list (comma-separated). Default: Moon.\n");
    printf("  --target <name(s)>    Alias for --target-body.\n");
    printf("  --aspect <name(s)>    Aspect list (comma-separated). Default: all aspects.\n");
    printf("  --list-aspects        Print indexed aspect list and exit.\n\n");
    print_aspect_list();
    printf("\nExamples:\n");
    printf("  astro predict transits --body Sun,Mars --target Moon,Venus --aspect 1,4\n");
    printf("  astro predict transits --start 2025-12-24 --days 30 --body Jupiter --target Moon\n");
}

int handle_predict_command(int argc, char *argv[]) {
    if (argc < 2) {
        print_predict_usage();
        return 1;
    }

    char* subcommand = argv[1];

    if (strcmp(subcommand, "transits") == 0) {
        const char* opt_start = get_option(argc, argv, "--start");
        const char* opt_end = get_option(argc, argv, "--end");
        const char* opt_days = get_option(argc, argv, "--days");
        const char* opt_body = get_option(argc, argv, "--body");
        const char* opt_target_body = get_option(argc, argv, "--target-body");
        if (!opt_target_body) {
            opt_target_body = get_option(argc, argv, "--target");
        }
        const char* opt_aspect = get_option(argc, argv, "--aspect");

        if (has_flag(argc, argv, "--list-aspects")) {
            print_aspect_list();
            return 0;
        }

        Date start_date, end_date;
        if (get_current_date(&start_date) != 0) {
            fprintf(stderr, "Error: Failed to determine current date.\n");
            return 1;
        }

        if (opt_start && parse_date_string(opt_start, &start_date) != 0) {
            fprintf(stderr, "Error: Invalid start date format (YYYY-MM-DD).\n");
            return 1;
        }

        int range_days = 30;
        if (opt_days) {
            char* endptr = NULL;
            long parsed_days = strtol(opt_days, &endptr, 10);
            if (!endptr || *endptr != '\0' || parsed_days <= 0 || parsed_days > 3660) {
                fprintf(stderr, "Error: Invalid --days value. Use a positive integer.\n");
                return 1;
            }
            range_days = (int)parsed_days;
        }

        if (opt_end) {
            if (parse_date_string(opt_end, &end_date) != 0) {
                fprintf(stderr, "Error: Invalid end date format (YYYY-MM-DD).\n");
                return 1;
            }
        } else {
            end_date = start_date;
            add_days(&end_date, range_days);
        }

        if (compare_dates(&start_date, &end_date) > 0) {
            fprintf(stderr, "Error: Start date cannot be after end date.\n");
            return 1;
        }

        char bodies[MAX_LIST_ITEMS][MAX_NAME_LEN];
        char targets[MAX_LIST_ITEMS][MAX_NAME_LEN];
        char aspects[MAX_ASPECTS][MAX_NAME_LEN];
        int body_count = 0;
        int target_count = 0;
        int aspect_count = 0;

        if (opt_body) {
            body_count = parse_csv_list(opt_body, bodies, MAX_LIST_ITEMS);
            if (body_count == 0) {
                fprintf(stderr, "Error: Invalid --body list.\n");
                return 1;
            }
        } else {
            snprintf(bodies[0], MAX_NAME_LEN, "Sun");
            body_count = 1;
        }

        if (opt_target_body) {
            target_count = parse_csv_list(opt_target_body, targets, MAX_LIST_ITEMS);
            if (target_count == 0) {
                fprintf(stderr, "Error: Invalid --target list.\n");
                return 1;
            }
        } else {
            snprintf(targets[0], MAX_NAME_LEN, "Moon");
            target_count = 1;
        }

        if (opt_aspect) {
            aspect_count = parse_aspect_list(opt_aspect, aspects, MAX_ASPECTS);
            if (aspect_count <= 0) {
                fprintf(stderr, "Error: Invalid --aspect list. Use --list-aspects for valid values.\n");
                return 1;
            }
        } else {
            aspect_count = load_all_aspects(aspects, MAX_ASPECTS);
        }

        // Get natal chart data from config
        char natal_time_str[MAX_VALUE_LEN] = "0:00";
        char natal_tz_str[MAX_VALUE_LEN] = "-12";
        char natal_loc_str[MAX_VALUE_LEN] = "176:14E 38:08S";
        char natal_lon_str[MAX_VALUE_LEN];
        char natal_lat_str[MAX_VALUE_LEN];

        config_get_value("natal.time", natal_time_str, sizeof(natal_time_str));
        config_get_value("natal.tz", natal_tz_str, sizeof(natal_tz_str));
        config_get_value("natal.location", natal_loc_str, sizeof(natal_loc_str));
        
        // Parse natal_loc_str into lon and lat
        if (sscanf(natal_loc_str, "%s %s", natal_lon_str, natal_lat_str) != 2) {
            fprintf(stderr, "Error: Invalid natal.location format in config. Expected \"LON LAT\".\n");
            return 1;
        }


        char start_buf[11];
        char end_buf[11];
        date_to_string(&start_date, start_buf, sizeof(start_buf));
        date_to_string(&end_date, end_buf, sizeof(end_buf));
        printf("Searching for transits from %s to %s for %d body/target combinations and %d aspect(s)...\n\n",
            start_buf, end_buf, body_count * target_count, aspect_count);

        Date current_date = start_date;
        char date_buf[11]; // YYYY-MM-DD\0

        char astrolog_args[ASTROLOG_ARGS_MAX];
        char raw_output_buffer[OUTPUT_BUFFER_SIZE];

        while (compare_dates(&current_date, &end_date) <= 0) {
            date_to_string(&current_date, date_buf, sizeof(date_buf));
            
            // Convert date MM-DD-YYYY to MM DD YYYY for astrolog
            int transit_year, transit_month, transit_day;
            if (sscanf(date_buf, "%d-%d-%d", &transit_year, &transit_month, &transit_day) != 3) {
                fprintf(stderr, "Internal error: Could not parse date_buf: %s\n", date_buf);
                return 1;
            }
            char final_transit_date_str[MAX_VALUE_LEN];
            snprintf(final_transit_date_str, sizeof(final_transit_date_str), "%d %d %d", transit_month, transit_day, transit_year);


            // Constructing natal chart params. This needs to come from the main config.
            char natal_date_str[MAX_VALUE_LEN] = "1972-04-08";
            config_get_value("natal.date", natal_date_str, sizeof(natal_date_str));
            int nat_year, nat_month, nat_day;
            if (sscanf(natal_date_str, "%d-%d-%d", &nat_year, &nat_month, &nat_day) != 3) {
                 nat_month = 4; nat_day = 8; nat_year = 1972; // Default if config missing
            }
            char final_natal_date_str[MAX_VALUE_LEN];
            snprintf(final_natal_date_str, sizeof(final_natal_date_str), "%d %d %d", nat_month, nat_day, nat_year);


      // Build command: load natal chart, then compute transits for the day, then list aspects.
      snprintf(astrolog_args, sizeof(astrolog_args),
        "-qa %s %s %s %s %s -tpd %s -a",
        final_natal_date_str, natal_time_str, natal_tz_str, natal_lon_str, natal_lat_str,
        final_transit_date_str);
            
            if (run_astrolog_command(astrolog_args, raw_output_buffer, OUTPUT_BUFFER_SIZE) == 0) {
                AspectInfo current_aspects[MAX_PLANETS * 2];
                int aspect_count = parse_aspect_output(raw_output_buffer, current_aspects, MAX_PLANETS * 2);

                for (int i = 0; i < aspect_count; i++) {
                    if (name_in_list(current_aspects[i].body1, bodies, body_count) &&
                        name_in_list(current_aspects[i].body2, targets, target_count) &&
                        aspect_in_list(current_aspects[i].aspect, aspects, aspect_count)) {
                        printf("MATCH: %s - %s %s %s on %s\n",
                            date_buf,
                            current_aspects[i].body1,
                            current_aspects[i].aspect,
                            current_aspects[i].body2,
                            date_buf); // Redundant date_buf but makes output clear
                    }
                }
            } else {
                fprintf(stderr, "Error: Failed to get aspects for date %s\n", date_buf);
            }
            
            increment_date(&current_date);
        }

    } else if (strcmp(subcommand, "returns") == 0) {
        printf("Placeholder: Predict Returns command recognized.\n");
    } else if (strcmp(subcommand, "stations") == 0) {
        printf("Placeholder: Predict Stations command recognized.\n");
    } else {
        printf("Unknown predict subcommand: %s\n\n", subcommand);
        print_predict_usage();
        return 1;
    }

    return 0;
}
