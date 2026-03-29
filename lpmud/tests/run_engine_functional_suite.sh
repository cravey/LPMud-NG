#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
TEST_DIR="$ROOT_DIR/tests"
source "$TEST_DIR/test_user.sh"
RUN_ID="$(date +%Y%m%d_%H%M%S)"
RUN_DIR_REL="tests/reports/engine_suite_${RUN_ID}"
RUN_DIR="$ROOT_DIR/$RUN_DIR_REL"
STATIC_DIR_REL="$RUN_DIR_REL/static"
STATIC_DIR="$ROOT_DIR/$STATIC_DIR_REL"

mkdir -p "$RUN_DIR" "$STATIC_DIR"

TEST_USER="$(make_test_user)"
TEST_PASS="codex_pass_123"

RUNTIME_ROOT="$(mktemp -d "${TMPDIR:-/tmp}/lpmud-runtime.XXXXXX")"
RUNTIME_MUDLIB="$RUNTIME_ROOT/mudlib"

PARSE_PID=""

cleanup() {
  set +e
  if [[ -n "$PARSE_PID" ]]; then
    kill "$PARSE_PID" >/dev/null 2>&1 || true
    wait "$PARSE_PID" >/dev/null 2>&1 || true
  fi
}
trap cleanup EXIT

echo "==> Building parse"
make -C "$ROOT_DIR" parse >"$RUN_DIR/make_parse.log" 2>&1

echo "==> Running static mudlib analysis"
pushd "$ROOT_DIR" >/dev/null
python3 "$TEST_DIR/analyze_mudlib.py" \
  --mudlib-root "mudlib" \
  --output-dir "$STATIC_DIR_REL" \
  >"$RUN_DIR/static_analysis.log" 2>&1
popd >/dev/null

echo "==> Preparing isolated runtime mudlib at $RUNTIME_MUDLIB"
cp -R "$ROOT_DIR/mudlib" "$RUNTIME_MUDLIB"

echo "==> Ensuring no stale parse listener is running"
STALE_PIDS="$(lsof -t -nP -iTCP:2000 -sTCP:LISTEN 2>/dev/null || true)"
if [[ -n "$STALE_PIDS" ]]; then
  kill $STALE_PIDS >/dev/null 2>&1 || true
  sleep 1
fi

echo "==> Starting parse"
pushd "$ROOT_DIR" >/dev/null
MUD_BIND_ADDR="127.0.0.1" MUD_LIB="$RUNTIME_MUDLIB" ./parse >"$RUN_DIR/parse.log" 2>&1 &
PARSE_PID=$!
popd >/dev/null

echo "==> Waiting for localhost:2000"
READY=0
for _ in $(seq 1 60); do
  if nc -z 127.0.0.1 2000 >/dev/null 2>&1; then
    READY=1
    break
  fi
  sleep 1
done
if [[ "$READY" -ne 1 ]]; then
  echo "parse did not open localhost:2000 in time" >&2
  tail -n 120 "$RUN_DIR/parse.log" >&2 || true
  exit 1
fi

echo "==> Running nc/expect session: new player"
expect "$TEST_DIR/nc_engine_suite.exp" \
  --host 127.0.0.1 \
  --port 2000 \
  --user "$TEST_USER" \
  --pass "$TEST_PASS" \
  --scenario new \
  --transcript "$RUN_DIR/session_new.log" \
  >"$RUN_DIR/session_new.expect.log" 2>&1

echo "==> Running nc/expect session: existing player"
expect "$TEST_DIR/nc_engine_suite.exp" \
  --host 127.0.0.1 \
  --port 2000 \
  --user "$TEST_USER" \
  --pass "$TEST_PASS" \
  --scenario existing \
  --transcript "$RUN_DIR/session_existing.log" \
  >"$RUN_DIR/session_existing.expect.log" 2>&1

echo "==> Writing summary"
cat >"$RUN_DIR/SUMMARY.txt" <<EOF
Engine functional suite completed.

Run ID: $RUN_ID
Reports: $RUN_DIR_REL
Test user: $TEST_USER

Artifacts:
- make_parse.log
- parse.log
- session_new.log
- session_existing.log
- static/monster_weapon_balance.md
- static/lpc_feature_usage.md
EOF

echo "==> Done: $RUN_DIR_REL"
