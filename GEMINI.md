# Astrolog 7.60

## Project Overview

This project contains the source code for Astrolog 7.60, a comprehensive and highly configurable astrology software program. It is written in C++ using a procedural, C-style approach and is designed to be cross-platform, with support for Unix/Linux, Windows, DOS, and older Mac systems.

The application is primarily a command-line tool that can generate a vast array of astrological charts, tables, and calculations. It supports various computational models, including the high-precision Swiss Ephemeris. Output can be rendered as text in the console or as graphics in formats like X11 bitmaps, PostScript, and Windows Metafiles.

The codebase is heavily configured through preprocessor macros in the central `astrolog.h` header file, which allows compiling different versions with specific features (e.g., with or without graphics, with different ephemeris engines) for various operating systems.

## Building and Running

The project uses a standard `Makefile` for compilation on Unix-like systems.

### Dependencies

- A C++ compiler (e.g., `g++` or `clang++`, often available as `cc`).
- The X11 library (`libX11`) is required for the graphical interface.
- The dynamic linking library (`libdl`).

On a Debian/Ubuntu-based system, these can be installed with:
`sudo apt-get install build-essential libx11-dev`

### Compilation

To compile the project, run the `make` command. This will compile all `.cpp` source files and link them into a single executable named `astrolog`.

```sh
make
```

This is equivalent to the manual steps:
```sh
# 1. Compile all source files into object files
c++ -c -O -Wno-write-strings -Wno-narrowing -Wno-comment *.cpp

# 2. Link the object files into the final executable
c++ -o astrolog *.o -lm -lX11 -ldl
```

### Running the Application

Once compiled, the application can be run from the command line:

```sh
./astrolog
```

Running it without arguments will likely prompt for user input or enter an interactive mode. The program is controlled by a large number of command-line switches.

**Key Command-Line Switches:**
*   **-H**: Display help and a list of available switches.
*   **-i <file>**: Read chart data from a file.
*   **-o <file>**: Write chart data to a file.
*   **-q <date/time info>**: Specify chart data directly on the command line.
*   **-v**: Display a standard text listing of planet and house positions.
*   **-w**: Display a text-based wheel chart.
*   **-g**: Display a text-based aspect grid.
*   **-X**: Open a graphical chart in an X11 window (if compiled with X11 support).

### Cleaning Up

To remove the compiled object files and the executable, run:

```sh
make clean
```

## Development Conventions

*   **Coding Style:** The code follows a C-like procedural style. It uses PascalCase for many type names and functions (e.g., `ChartListing`, `UserSettings`) and a form of Hungarian notation in places (e.g., `f` for flag, `sz` for string, `cch` for character count). Global variables are used extensively to manage state and settings.
*   **Configuration:** The primary configuration is managed via `#define` macros in `astrolog.h`. To enable or disable major features or target a specific platform, this file must be edited before compilation. For example, commenting out `#define X11` would build a version without the X11 graphical interface.
*   **File Structure:**
    *   `astrolog.h`: Central header defining most data structures, constants, and compilation flags.
    *   `astrolog.cpp`: Contains the `main` function, switch processing, and the main program loop.
    *   `charts*.cpp`: Source files for generating text-based charts.
    *   `xcharts*.cpp`: Source files for X11 graphics.
    *   `calc.cpp`, `placalc.cpp`, `swe*.cpp`: Various calculation engines.
    *   `io.cpp`, `data.cpp`: Data and file input/output.
*   **Extensibility:** The program appears to be designed with some level of extensibility in mind, particularly through its "AstroExpression" system (`-~` switches) which allows for custom-defined formatting and logic.
