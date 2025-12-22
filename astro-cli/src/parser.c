#include "parser.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h> // For atoi
#include <ctype.h>  // For isspace

// --- Helper functions for parsing astrolog -v output ---

// Skips lines in the buffer until it finds one containing "Body  Locat."
const char* find_body_header(const char* buffer) {
    const char* line = buffer;
    while (line) {
        if (strstr(line, "Body  Locat.") != NULL) {
            // Skip the header line itself
            const char* next_line = strchr(line, '\n');
            return next_line ? next_line + 1 : NULL;
        }
        line = strchr(line, '\n');
        if (line) line++;
    }
    return NULL;
}

int parse_v_output(const char* raw_output, PlanetInfo* planets, int max_planets) {
    int count = 0;
    const char* line = find_body_header(raw_output);
    if (!line) {
        return 0;
    }

    char buffer[256];

    while (line && count < max_planets) {
        const char* next_line = strchr(line, '\n');
        size_t line_len = next_line ? (size_t)(next_line - line) : strlen(line);

        if (line_len == 0 || line_len > sizeof(buffer) -1) {
            line = next_line ? next_line + 1 : NULL;
            continue;
        }
        
        memcpy(buffer, line, line_len);
        buffer[line_len] = '\0';

        // Stop if we reach the element table summary
        if (strstr(buffer, "Car Fix Mut TOT") != NULL) {
            break;
        }
        
        // A very basic check to see if it's a planet line
        if (buffer[4] != ':') {
             line = next_line ? next_line + 1 : NULL;
             continue;
        }

        PlanetInfo p;
        memset(&p, 0, sizeof(PlanetInfo));

        // Extract Name
        char name_buf[10];
        strncpy(name_buf, buffer, 4);
        name_buf[4] = '\0';
        // trim whitespace
        char* end = name_buf + strlen(name_buf) - 1;
        while(end > name_buf && isspace((unsigned char)*end)) end--;
        *(end + 1) = 0;
        snprintf(p.name, sizeof(p.name), "%s", name_buf);

        // Extract Degrees, Sign, Minutes
        sscanf(buffer + 6, "%d%3s%d", &p.degrees, p.sign, &p.minutes);

        // A more robust check for the 'R' for retrograde
        p.is_retrograde = (buffer[14] == 'R') ? 'R' : ' ';

        // Extract House using a more robust method
        const char* house_marker = strstr(buffer, "[");
        if (house_marker) {
            p.house = atoi(house_marker + 1);
        }
        
        planets[count++] = p;        
        line = next_line ? next_line + 1 : NULL;
    }

    return count;
}


void output_csv(const PlanetInfo* planets, int count) {
    // Print header
    printf("Body,Sign,Degrees,Minutes,IsRetrograde,House\n");

    // Print data
    for (int i = 0; i < count; i++) {
        printf("%s,%s,%d,%d,%c,%d\n",
            planets[i].name,
            planets[i].sign,
            planets[i].degrees,
            planets[i].minutes,
            planets[i].is_retrograde,
            planets[i].house);
    }
}

// --- Helper functions for parsing astrolog -a output ---

// Skips lines until it finds the aspect list header (e.g., "  1:     Sun")
const char* find_aspect_header(const char* buffer) {
    const char* line = buffer;
    while (line) {
        // Look for the first aspect line which starts with "  1:"
        // And ensure it's not a time entry (which also has a colon)
        if (strstr(line, "  1:") != NULL && (strchr(line, ':') < strchr(line, '(') || strchr(line, '(') == NULL) ) {
            return line;
        }
        line = strchr(line, '\n');
        if (line) line++; // Move to the beginning of the next line
    }
    return NULL;
}

int parse_aspect_output(const char* raw_output, AspectInfo* aspects, int max_aspects) {
    int count = 0;
    const char* line = find_aspect_header(raw_output);
    if (!line) {
        return 0; // No aspect list found
    }

    char buffer[256];
    char temp_body1[MAX_NAME_LEN];
    char temp_aspect[MAX_NAME_LEN];
    char temp_body2[MAX_NAME_LEN];
    int index;

    while (line && count < max_aspects) {
        const char* next_line = strchr(line, '\n');
        size_t line_len = next_line ? (size_t)(next_line - line) : strlen(line);

        if (line_len == 0 || line_len > sizeof(buffer) - 1) {
            line = next_line ? next_line + 1 : NULL;
            continue;
        }
        
        strncpy(buffer, line, line_len);
        buffer[line_len] = '\0';

        // Stop if we reach the summary
        if (strstr(buffer, "Sum power:") != NULL || strlen(buffer) < 10) { // Also stop on short lines that are not aspects
            break;
        }
        
        // Use a pointer to advance through the buffer as tokens are consumed
        const char* current_pos = buffer;
        int chars_read = 0;
        int scan_count;

        // Skip the leading index and colon, e.g., "  1:"
        // Pattern: Optional spaces, index, colon, spaces
        scan_count = sscanf(current_pos, " %d: %n", &index, &chars_read);
        if (scan_count != 1) {
             line = next_line ? next_line + 1 : NULL;
             continue; // Not a valid aspect line start
        }
        current_pos += chars_read;

        // Read Body1 (up to MAX_NAME_LEN-1 characters, stopping at space or '(')
        scan_count = sscanf(current_pos, "%s%n", temp_body1, &chars_read);
        if (scan_count != 1) {
             line = next_line ? next_line + 1 : NULL;
             continue;
        }
        current_pos += chars_read;

        // Skip whitespace, then optional (Sign1), then any trailing space.
        while (*current_pos != '\0' && isspace((unsigned char)*current_pos)) {
            current_pos++;
        }
        if (*current_pos == '(') {
            const char* closing_paren = strchr(current_pos, ')');
            if (closing_paren) {
                current_pos = closing_paren + 1;
            } else { // Malformed sign, skip line
                line = next_line ? next_line + 1 : NULL;
                continue;
            }
        }
        while (*current_pos != '\0' && isspace((unsigned char)*current_pos)) {
            current_pos++;
        }

        // Read Aspect
        scan_count = sscanf(current_pos, "%s%n", temp_aspect, &chars_read);
        if (scan_count != 1) {
             line = next_line ? next_line + 1 : NULL;
             continue;
        }
        current_pos += chars_read;

        // Skip whitespace, then optional (Sign2), then any trailing space.
        while (*current_pos != '\0' && isspace((unsigned char)*current_pos)) {
            current_pos++;
        }
        if (*current_pos == '(') {
            const char* closing_paren = strchr(current_pos, ')');
            if (closing_paren) {
                current_pos = closing_paren + 1;
            } else { // Malformed sign, skip line
                line = next_line ? next_line + 1 : NULL;
                continue;
            }
        }
        while (*current_pos != '\0' && isspace((unsigned char)*current_pos)) {
            current_pos++;
        }

        // Read Body2
        scan_count = sscanf(current_pos, "%s%n", temp_body2, &chars_read);
        if (scan_count != 1) {
             line = next_line ? next_line + 1 : NULL;
             continue;
        }
        
        // If we reached here, parsing was successful enough.
        strncpy(aspects[count].body1, temp_body1, sizeof(aspects[count].body1) - 1);
        aspects[count].body1[sizeof(aspects[count].body1) - 1] = '\0';
        strncpy(aspects[count].aspect, temp_aspect, sizeof(aspects[count].aspect) - 1);
        aspects[count].aspect[sizeof(aspects[count].aspect) - 1] = '\0';
        strncpy(aspects[count].body2, temp_body2, sizeof(aspects[count].body2) - 1);
        aspects[count].body2[sizeof(aspects[count].body2) - 1] = '\0';
        count++;
        
        line = next_line ? next_line + 1 : NULL;
    }

    return count;
}
