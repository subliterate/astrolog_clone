#!/usr/bin/env bash
# Minimal stub used by unit and integration tests to avoid invoking the real astrolog binary.
echo "stub astrolog args: $*"

if [[ "$*" == *"-a"* ]]; then
  cat <<'EOF'
  1: Sun (Ari) Con Neptune (Cap)
  2: Mars (Gem) Opp Jupiter (Sag)
Sum power:
EOF
elif [[ "$*" == *"-v"* ]]; then
  cat <<'EOF'
Body  Locat.
Sun :  5Ari20 R [02]
Moon: 17Lib05   [07]
Car Fix Mut TOT
EOF
else
  echo "stub chart output"
fi
