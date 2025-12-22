# Astro-CLI Analysis & Test Regime

## Overview
- `astro-cli` wraps the legacy `astrolog` binary with commands for `chart`, `predict`, `data`, and `config`, layered as `main` → `command` (business rules) → `engine` (process launch) → `parser` (screen-scrape helpers) plus `config`/`date_util`.
- Default CLI behaviour still depends on the real `astrolog` executable being on `PATH` or provided via `ASTROLOG_BIN`. Tests now default to a bundled stub to avoid platform GLIBC version mismatches.

## Consistency Review
- **Architecture**: Separation of concerns is clear, but command handlers duplicate date/location/config hydration logic; consider extracting helpers to keep behaviour aligned across `data`/`chart`/`predict`.
- **Style**: Mostly 4-space indents but with stray 2-space blocks; snake_case naming is consistent inside `astro-cli` yet differs from the legacy PascalCase macro style in the root project.
- **Option parsing**: `get_option` is minimal (single-pass, no validation of missing option values, no support for repeated flags). Subcommand usage printing is good, but argument validation is uneven across commands.
- **Buffers & safety**: Several `strncpy` calls skip explicit null termination when input length meets the buffer limit; switching to `snprintf` or forcing `buf[n-1] = '\0'` would harden against overlong input. `config_set_value` uses `system("mkdir -p")` instead of `mkdir`, which is fragile and inherits shell injection risk via `$HOME`.
- **Engine exit codes**: `run_astrolog_command` discards the child exit status, so failing `astrolog` invocations may still be reported as success.
- **Parser brittleness**: `parser.c` relies on column positions (`buffer[14] == 'R'` and fixed-width slicing) and simplistic header detection; any upstream format change will silently drop rows or mis-flag retrograde states. Aspect parsing repeatedly scans lines with `strchr`/`strstr`, leaving room for edge-case mis-detection.

## Test Regime
### Unit tests
- `tests/test_date_util.c`: Validates parsing/formatting, increment, and comparisons (including leap year rollover).
- NEW `tests/test_config.c`: Temp `HOME` sandbox for config round-trip, update-in-place, and missing-key handling.
- NEW `tests/test_parser.c`: Exercises `-v` body parsing and `-a` aspect parsing with fixtures matching current scraping assumptions.
- NEW `tests/test_engine.c`: Verifies `run_astrolog_command` honours `ASTROLOG_BIN` and captures stubbed output.

### Integration tests (`tests/smoke.sh`)
- Defaults `ASTROLOG_BIN` to `tests/fake_astrolog.sh`, which emits predictable `-v` and `-a` output so the CLI paths for `help`, `config`, `data positions --output csv`, `chart natal`, and `predict transits` all execute deterministically.
- Optional real-binary check is gated by `CHECK_REAL_ASTROLOG=1` and `REAL_ASTROLOG_BIN=/path/to/astrolog` to avoid GLIBC version issues on hosts where the bundled binary cannot run.

### How to run
- Full suite (executed): `cd astro-cli && make clean && make test`
  - Uses the stub by default; override with `make ASTROLOG_BIN=/path/to/astrolog CHECK_REAL_ASTROLOG=1 integration-test` to hit the real binary.

## Recommendations
- Harden string handling: replace `strncpy` uses with bounded `snprintf` or explicit terminators; validate option presence/length before copy.
- Normalize shared chart setup into helpers to cut duplication and reduce divergence between `data`, `chart`, and `predict`.
- Make parser more resilient (tokenize on whitespace, tolerate variable spacing, and add negative tests for malformed rows); add fixtures sourced from real `astrolog` output once GLIBC compatibility is resolved.
- Replace `system("mkdir -p")` with `mkdir`/`stat` to avoid shell dependency and improve portability; capture `pclose`/child exit codes in `run_astrolog_command` for accurate error reporting.
