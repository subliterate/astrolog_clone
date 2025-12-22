# Project Summary

## Overview
- Root contains the upstream Astrolog 7.60 source (April 9, 2023 release) with classic C/C++ modules (`astrolog.cpp`, `charts*.cpp`, `xcharts*.cpp`, calculation files, headers `astrolog.h`/`extern.h`).
- Ephemeris/star data live under `ephem/` with an alternate copy `astephem/`; file names such as `.se1` are expected as-is.
- Build artifacts for platforms include `Makefile` (Unix/X11), Visual Studio solutions/projects, and helper script `astrolog.sh`.
- `astro-cli/` is a modern C wrapper offering friendlier subcommands atop the legacy binary; it has its own `Makefile`, `src/`, `build/`, `bin/`, and `tests/`.
- Docs: `README.md` notes the repository mirrors upstream 7.60; `GEMINI.md` gives a high-level intro; `DESIGN.md` describes Astro-CLI goals and module roles.

## Build & Run
- Classic engine: `make` in repo root builds `./astrolog` (links X11 and `dl` by default); clean via `make clean`. Quick smoke: `./astrolog -qa 1 1 2000 12:00 -z0 -v`.
- Astro-CLI: `make -C astro-cli` builds `astro-cli/bin/astro`; clean via `make -C astro-cli clean`. Smoke script `astro-cli/tests/smoke.sh` exercises help, config set/get, data positions CSV, chart natal, transit matching, and a direct astrolog probe using a temp HOME.

## Astro-CLI Implementation
- `src/main.c` dispatches `chart`, `predict`, `data`, `config`, `help`.
- `src/command.c`:
  - `data positions` and `chart natal` assemble `-qa` calls using date/time/tz/location from CLI opts or `~/.config/astro/config.ini`; location is split into lon/lat tokens.
  - `predict transits` parses a date range, iterates daily, loads natal defaults, runs `-tpd ... -a`, parses aspects, and prints matches on simple string equality.
  - `predict returns` and `predict stations` currently print placeholders.
- `src/config.c` manages a simple key/value config file; ensures `~/.config/astro/` exists (via `mkdir -p` shell call) and rewrites on set.
- `src/date_util.c` handles YYYY-MM-DD parsing/formatting, comparisons, and day-by-day increments.
- `src/parser.c` converts `-v` output to structs/CSV and parses `-a` aspect listings.
- `src/engine.c` runs the legacy binary through `popen`, but the path is hardcoded to `/home/terry/astrolog/astrolog`; adjust to your local `./astrolog` or env-based path before use.

## Notable Gaps/Risks
- Astro-CLI transit matching uses naïve string comparisons; no fuzzy name/aspect mapping yet.
- Predict returns/stations are unimplemented.
- Hardcoded astrolog path in `src/engine.c` will break on most machines until changed.
- No automated tests beyond the smoke script; the classic engine remains the upstream codebase with global/macro configuration via `astrolog.h`.
