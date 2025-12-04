# Repository Guidelines

## Project Structure & Module Organization
- Core C/C++ sources live in the repository root (`astrolog.cpp`, `charts*.cpp`, `xcharts*.cpp`, calc modules, shared headers such as `astrolog.h`, `extern.h`).
- Ephemeris and star data are stored under `ephem/` (primary) and `astephem/` (alternate copy). Keep `.se1` filenames unchanged; code paths expect them verbatim.
- Platform build files: `Makefile` (Unix/X11), `Astrolog.sln` / `Astrolog.vcxproj` (Windows), helper script `astrolog.sh`.
- Modern CLI wrapper resides in `astro-cli/` with its own `Makefile`, `src/`, `build/`, and `bin/` directories.

## Build, Test, and Development Commands
- Build classic binary: `make` (outputs `./astrolog`, links X11 and `dl` by default).
- Clean artifacts: `make clean`.
- Run basics: `./astrolog -H` (switch help), `./astrolog -v` (text listing), `./astrolog -X` (X11 chart when compiled with graphics).
- Deterministic smoke test: `./astrolog -qa 1 1 2000 12:00 -z0 -v`.
- Build CLI wrapper: `make -C astro-cli` → `astro-cli/bin/astro`; clean via `make -C astro-cli clean`. Quick check: `./astro-cli/bin/astro help`.

## Coding Style & Naming Conventions
- Preserve the existing C-style, procedural approach; avoid introducing modern C++ constructs unless essential for a fix.
- Indent with two spaces, K&R braces, and reuse the banner/block comment style already present.
- Follow naming patterns: PascalCase for major routines, prefix flags with `f`, strings with `sz`, counters with `c`, arrays with `rg`, and constants/macros in ALL_CAPS.
- Configure features through `astrolog.h` macros (e.g., toggling X11) instead of scattering new compile-time switches elsewhere.

## Testing Guidelines
- No automated suite exists; rely on manual runs. Rebuild from clean (`make clean && make`) before validation.
- If you touch rendering, verify `-v` text output and an X11 smoke test (`-X`) on a machine with an X server.
- For `astro-cli` changes, ensure each subcommand prints `--help` correctly and exits 0; document any new examples in your PR.

## Commit & Pull Request Guidelines
- Use short, imperative commit titles (e.g., `Fix chart wheel redraw`); link issues when applicable. Existing history is minimal—be explicit about intent.
- PRs should state what changed, commands executed, and expected/observed output (include screenshots only when UI/X11 behavior changed).
- Do not strip or alter licensing headers, and avoid renaming `.se1` data files or moving ephemeris directories, as paths are hard-coded.
