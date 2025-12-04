#!/usr/bin/env bash
set -euo pipefail

# Simple end-to-end smoke suite for astro-cli.
# Uses a temp HOME so it doesn't touch the user's real config.

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
ASTRO_BIN="$ROOT/bin/astro"
ASTROLOG_BIN="$ROOT/../astrolog"

if [[ ! -x "$ASTRO_BIN" ]]; then
  echo "astro binary not built; run 'make' first" >&2
  exit 1
fi

TMPHOME="$(mktemp -d)"
trap 'rm -rf "$TMPHOME"' EXIT
export HOME="$TMPHOME"

pass() { printf "[PASS] %s\n" "$1"; }
fail() { printf "[FAIL] %s\n" "$1"; exit 1; }

# 1) help wiring
$ASTRO_BIN help | grep -q "Usage: astro" && pass "help prints usage" || fail "help"

# 2) config writes/reads (temp HOME)
$ASTRO_BIN config set natal.date 1990-01-01 >/dev/null
$ASTRO_BIN config set natal.time 12:00 >/dev/null
$ASTRO_BIN config set natal.tz 0 >/dev/null
$ASTRO_BIN config set natal.location "0W00 51N30" >/dev/null

# 3) data positions CSV parses Sun line
$ASTRO_BIN data positions --date 2000-01-01 --time 12:00 --tz 0 --loc "0W00 51N30" --output csv | grep -q "Sun" && pass "data positions outputs CSV" || fail "data positions"

# 4) chart command runs
$ASTRO_BIN chart natal --date 2000-01-01 --time 12:00 --tz 0 --loc "0W00 51N30" >/dev/null && pass "chart natal runs" || fail "chart natal"

# 5) transit match detection (uses defaults set above)
TRANSIT_OUT="$($ASTRO_BIN predict transits --start 2024-01-01 --end 2024-01-01 --body Sun --target-body Neptune --aspect Con)"
printf "%s" "$TRANSIT_OUT" | grep -q "MATCH:" && pass "predict transits finds match" || fail "predict transits"

# Optional: ensure underlying astrolog reachable with non-interactive chart
$ASTROLOG_BIN -qa 1 1 2000 12:00 0 0 0 -v >/dev/null 2>&1 && pass "astrolog reachable" || fail "astrolog binary"
