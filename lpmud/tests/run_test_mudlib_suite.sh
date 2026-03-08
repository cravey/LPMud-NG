#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
TEST_DIR="$ROOT_DIR/tests"
RUN_ID="$(date +%Y%m%d_%H%M%S)"
RUN_DIR_REL="tests/reports/test_mudlib_suite_${RUN_ID}"
RUN_DIR="$ROOT_DIR/$RUN_DIR_REL"
mkdir -p "$RUN_DIR"

TEST_USER="$(uuidgen | tr '[:upper:]' '[:lower:]' | tr -d '-' | tr '0-9' 'abcdefghij' | cut -c1-10)"

RUNTIME_ROOT="$(mktemp -d "${TMPDIR:-/tmp}/lpmud-test-mudlib.XXXXXX")"
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

echo "==> Preparing isolated test mudlib runtime"
cp -R "$ROOT_DIR/test-mudlib" "$RUNTIME_MUDLIB"

echo "==> Ensuring localhost:2000 is free"
STALE_PIDS="$(lsof -t -nP -iTCP:2000 -sTCP:LISTEN 2>/dev/null || true)"
if [[ -n "$STALE_PIDS" ]]; then
  kill $STALE_PIDS >/dev/null 2>&1 || true
  sleep 1
fi

echo "==> Starting parse with test-mudlib"
pushd "$ROOT_DIR" >/dev/null
MUD_BIND_ADDR="127.0.0.1" MUD_LIB="$RUNTIME_MUDLIB" ./parse >"$RUN_DIR/parse.log" 2>&1 &
PARSE_PID=$!
popd >/dev/null

echo "==> Waiting for localhost:2000"
READY=0
for _ in $(seq 1 40); do
  if nc -z 127.0.0.1 2000 >/dev/null 2>&1; then
    READY=1
    break
  fi
  sleep 1
done
if [[ "$READY" -ne 1 ]]; then
  echo "parse did not open localhost:2000 in time" >&2
  tail -n 200 "$RUN_DIR/parse.log" >&2 || true
  exit 1
fi

echo "==> Running engine LPC suite through nc"
expect "$TEST_DIR/test_mudlib_suite.exp" \
  --host 127.0.0.1 \
  --port 2000 \
  --user "$TEST_USER" \
  --transcript "$RUN_DIR/session.log" \
  >"$RUN_DIR/expect.log" 2>&1

SUMMARY_LINE="$(rg -n 'TEST_SUMMARY total=' "$RUN_DIR/session.log" -N | tail -n 1 || true)"
RESULT_LINE="$(rg -n 'SUITE_RESULT total=' "$RUN_DIR/expect.log" -N | tail -n 1 || true)"

cat >"$RUN_DIR/SUMMARY.txt" <<EOF
Test mudlib suite completed.

Run ID: $RUN_ID
Run dir: $RUN_DIR_REL
Test user: $TEST_USER

Summary line:
$SUMMARY_LINE

Expect result:
$RESULT_LINE

Artifacts:
- make_parse.log
- parse.log
- expect.log
- session.log
EOF

echo "==> Done: $RUN_DIR_REL"
