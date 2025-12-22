#ifndef PARSER_H
#define PARSER_H

#define MAX_PLANETS 30
#define MAX_NAME_LEN 15

// Structure to hold the parsed information for one celestial body
typedef struct {
    char name[10];
    char sign[5];
    int degrees;
    int minutes;
    char is_retrograde; // 'R' or ' '
    int house;
} PlanetInfo;

typedef struct {
    char body1[15]; // e.g., "Sun", "Ascenda"
    char aspect[5]; // e.g., "Con", "Tri"
    char body2[15]; // e.g., "Midheaven", "Jupiter"
} AspectInfo;


// Parses the standard text output from an astrolog -v command.
// raw_output: The string buffer containing the raw text from astrolog.
// planets: An array of PlanetInfo structs to be filled.
// max_planets: The size of the planets array.
// Returns: The number of planets parsed.
int parse_v_output(const char* raw_output, PlanetInfo* planets, int max_planets);

// Prints an array of PlanetInfo structs in CSV format.
// planets: The array of PlanetInfo structs.
// count: The number of planets in the array.
void output_csv(const PlanetInfo* planets, int count);

// Parses the text output from an astrolog -a command.
// raw_output: The string buffer containing the raw text from astrolog.
// aspects: An array of AspectInfo structs to be filled.
// max_aspects: The size of the aspects array.
// Returns: The number of aspects parsed.
int parse_aspect_output(const char* raw_output, AspectInfo* aspects, int max_aspects);

#endif // PARSER_H
