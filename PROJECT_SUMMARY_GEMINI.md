# Project Summary: Astrolog 7.60 and Astro-CLI

This project workspace combines the legacy **Astrolog 7.60** engine with a modern wrapper called **Astro-CLI**.

### 1. Astrolog 7.60 (Core Engine)
*   **Description:** A comprehensive, procedural C++ astrology software originally released in 1991. It provides high-precision calculations (using Swiss Ephemeris and other models) and diverse outputs (text charts, X11 graphics, PostScript).
*   **Structure:**
    *   **Configuration:** Features like graphics support (`#define X11`) and platform specifics are toggled via macros in `astrolog.h`.
    *   **Build:** Uses a standard `Makefile` to compile the `astrolog` executable.
    *   **Role:** Acts as the computational backend for this workspace.

### 2. Astro-CLI (Modern Wrapper)
*   **Description:** A new C-based command-line tool designed to modernize the user experience of `astrolog`. It replaces complex command-line switches with intuitive subcommands (e.g., `astro chart natal`, `astro predict transits`).
*   **Key Features:**
    *   **Stateful Configuration:** Persists user details (location, birth data) to `~/.config/astro/config.ini`, eliminating repetitive input.
    *   **Structured Output:** Can parse `astrolog`'s text output into CSV for data analysis.
    *   **Architecture:**
        *   **`engine.c`:** Bridges to the legacy `astrolog` binary.
        *   **`parser.c`:** Converts legacy text output into structured C data.
        *   **`date_util.c`:** Handles date math for predictive features.
*   **Status:**
    *   **Complete:** Core infrastructure, configuration, and basic chart generation.
    *   **In Progress:** Predictive features (transits) and advanced aspect parsing.

### File Structure Highlights
*   **`astrolog.h`**: The central configuration header for the legacy engine.
*   **`astro-cli/src/`**: Source code for the modern wrapper (`main.c`, `command.c`, `engine.c`, etc.).
*   **`GEMINI.md` & `DESIGN.md`**: Project documentation and architecture details.
