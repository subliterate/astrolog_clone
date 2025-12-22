#!/usr/bin/env bash
# set -euo pipefail # Temporarily disable to debug hangs

# Simple end-to-end smoke suite for astro-cli.
# Uses a temp HOME so it doesn't touch the user's real config.

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
ASTRO_BIN="$ROOT/bin/astro"
ASTROLOG_BIN="${ASTROLOG_BIN:-$ROOT/tests/fake_astrolog.sh}" # Default to fake if not set
REAL_ASTROLOG_BIN="${REAL_ASTROLOG_BIN:-$ROOT/../astrolog}"

if [[ ! -x "$ASTRO_BIN" ]]; then
  echo "astro binary not built; run 'make' first" >&2
  exit 1
fi

# Run unit tests via the Makefile before the smoke checks.
make -C "$ROOT" test >/dev/null 2>&1 || fail "unit tests"

# This check is only for the REAL_ASTROLOG_BIN check at the end
# if [[ ! -x "$ASTROLOG_BIN" ]]; then
#   echo "astrolog binary not found at $ASTROLOG_BIN" >&2
#   exit 1
# fi

TMPHOME="$(mktemp -d)"
trap 'rm -rf "$TMPHOME"' EXIT
export HOME="$TMPHOME"
export ASTROLOG_BIN # This needs to be set for the astro-cli executable

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
echo "Executing: $ASTRO_BIN data positions --date 2000-01-01 --time 12:00 --tz 0 --loc \"0W00 51N30\" --output csv"
RAW_DATA_OUTPUT="$($ASTRO_BIN data positions --date 2000-01-01 --time 12:00 --tz 0 --loc "0W00 51N30" --output csv)"
echo "Raw output from data positions:"
echo "$RAW_DATA_OUTPUT"
printf "%s" "$RAW_DATA_OUTPUT" | grep -q "Sun" && pass "data positions outputs CSV" || fail "data positions"

# 4) chart command runs
$ASTRO_BIN chart natal --date 2000-01-01 --time 12:00 --tz 0 --loc "0W00 51N30" >/dev/null && pass "chart natal runs" || fail "chart natal"

# 5) transit match detection (uses defaults set above)
TRANSIT_OUT="$($ASTRO_BIN predict transits --start 2024-01-01 --end 2024-01-01 --body Sun --target-body Neptune --aspect Con)"
printf "%s" "$TRANSIT_OUT" | grep -q "MATCH:" && pass "predict transits finds match" || fail "predict transits"

if [[ "${CHECK_REAL_ASTROLOG:-0}" == "1" ]]; then
  if [[ -x "$REAL_ASTROLOG_BIN" ]]; then
    $REAL_ASTROLOG_BIN -qa 1 1 2000 12:00 0 0 0 -v >/dev/null 2>&1 && pass "real astrolog reachable" || fail "real astrolog binary"
  else
    echo "Skipping real astrolog check; REAL_ASTROLOG_BIN not executable ($REAL_ASTROLOG_BIN)" >&2
  fi
fi
