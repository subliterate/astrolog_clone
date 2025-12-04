# Astro-CLI: Design Scheme and Build Status

## 1. Design Philosophy

The **Astro-CLI** project is designed as a modern, user-friendly command-line interface (CLI) wrapper around the powerful, but complex, `astrolog` calculation engine.

*   **Abstraction:** It abstracts away the cryptic switch-based syntax of `astrolog` (e.g., `-v -qa ...`) into human-readable subcommands (e.g., `astro data positions`).
*   **Stateless & Configurable:** While individual commands are stateless, a configuration system allows users to persist their natal data and preferences, eliminating repetitive data entry.
*   **Data-Centric:** The tool prioritizes structured data output (CSV) alongside human-readable text, enabling downstream data analysis and integration with other tools.
*   **UNIX Philosophy:** It is designed to do one thing well (interface with astrological data) and work well via pipes and standard streams.

## 2. System Architecture

The application is written in C and follows a modular architecture:

*   **`main.c` (Entry Point):** Handles initial argument vector (`argv`) parsing and dispatches execution to specific command handlers based on the primary verb (`chart`, `predict`, `data`, `config`).
*   **`command.c` (Command Layer):** Contains the business logic for each subcommand. It parses specific flags (e.g., `--date`, `--body`), retrieves default values from the configuration, constructs the necessary `astrolog` command strings, and orchestrates execution.
*   **`engine.c` (Execution Layer):** Acts as the bridge to the legacy software. It executes the `astrolog` binary as a subprocess using `popen()`, handles the input/output streams, and captures the raw text output into memory buffers.
*   **`parser.c` (Translation Layer):** Parses the unstructured, fixed-width text output from `astrolog` into structured C structs (`PlanetInfo`, `AspectInfo`). This allows the CLI to filter, format, and converting data into CSV.
*   **`config.c` (Persistence Layer):** Manages a simple key-value store in `~/.config/astro/config.ini`, allowing users to save defaults for natal date, time, location, and timezone.
*   **`date_util.c` (Utility Layer):** Provides robust date parsing, validation, formatting, and arithmetic (iteration) logic to support predictive features.

## 3. Current Build Status

### Core Infrastructure
*   **Status:** **Complete**
*   **Details:**
    *   Directory structure is established (`src/`, `bin/`, `build/`).
    *   `Makefile` is fully functional, handling incremental builds and linking all modules.
    *   The engine module correctly locates and executes the absolute path of the `astrolog` executable.

### Configuration Management
*   **Status:** **Complete**
*   **Command:** `astro config [get|set] <key> [value]`
*   **Details:** Users can set and retrieve `natal.date`, `natal.time`, `natal.tz`, and `natal.location`. Values are persisted to disk.

### Data Extraction
*   **Status:** **Functional (Beta)**
*   **Command:** `astro data positions [options]`
*   **Details:**
    *   Retrieves planetary positions for any given date.
    *   Supports CSV output via `--output csv`.
    *   **Fix Applied:** Location parsing logic was updated to correctly split "LON LAT" strings into separate arguments to prevent "Unknown subswitch" errors in the underlying engine.

### Chart Generation
*   **Status:** **Functional**
*   **Command:** `astro chart natal [options]`
*   **Details:**
    *   Generates a text-based ASCII wheel chart.
    *   Fully supports configuration defaults and command-line overrides.
    *   **Fix Applied:** applied the same location string splitting fix as the data command.

### Predictive Astrology
*   **Status:** **In Progress / Partially Implemented**
*   **Command:** `astro predict transits [options]`
*   **Details:**
    *   Argument parsing for start/end dates, bodies, and aspects is complete.
    *   Date iteration logic (day-by-day) is implemented using `date_util`.
    *   **Aspect Parsing:** The `parse_aspect_output` function has been implemented to read `astrolog -a` lists.
    *   **Filtering:** Logic to filter aspects based on user input matches is implemented.
    *   **Current State:** The code compiles. The logic to interface with `astrolog` for transit calculation has been updated to correctly format location arguments.

### Pending Features
1.  **Predict Returns/Stations:** The command structure exists, but the logic handles are currently placeholders.
2.  **Extended Help:** While usage strings exist, a dedicated help system with examples is not yet implemented.
3.  **Advanced Parsing:** Parsing for retrograde stations and complex aspect patterns needs refinement.

## 4. Next Steps
1.  Run comprehensive integration tests on the `predict transits` command to ensure the location fix works as expected and transits are accurately identified.
2.  Implement the logic for `predict returns` and `predict stations`.
3.  Refine the CSV output to ensure all data fields (retrograde status, house) are 100% accurate across all edge cases.