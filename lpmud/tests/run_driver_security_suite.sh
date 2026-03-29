#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
RUN_ID="$(date +%Y%m%d_%H%M%S)"
RUN_DIR_REL="tests/reports/driver_security_suite_${RUN_ID}"
RUN_DIR="$ROOT_DIR/$RUN_DIR_REL"
RUNTIME_ROOT="$(mktemp -d "${TMPDIR:-/tmp}/lpmud-driver-security.XXXXXX")"
RUNTIME_MUDLIB="$RUNTIME_ROOT/mudlib"
PARSE_PID=""

PASS_COUNT=0
FAIL_COUNT=0
SKIP_COUNT=0

cleanup() {
  set +e
  if [[ -n "$PARSE_PID" ]]; then
    kill "$PARSE_PID" >/dev/null 2>&1 || true
    wait "$PARSE_PID" >/dev/null 2>&1 || true
    PARSE_PID=""
  fi
}
trap cleanup EXIT

mkdir -p "$RUN_DIR"

record_pass() {
  local id="$1"
  local msg="$2"
  PASS_COUNT=$((PASS_COUNT + 1))
  echo "[PASS][$id] $msg" | tee -a "$RUN_DIR/RESULTS.log"
}

record_fail() {
  local id="$1"
  local msg="$2"
  FAIL_COUNT=$((FAIL_COUNT + 1))
  echo "[FAIL][$id] $msg" | tee -a "$RUN_DIR/RESULTS.log"
}

record_skip() {
  local id="$1"
  local msg="$2"
  SKIP_COUNT=$((SKIP_COUNT + 1))
  echo "[SKIP][$id] $msg" | tee -a "$RUN_DIR/RESULTS.log"
}

stop_parse() {
  set +e
  if [[ -n "$PARSE_PID" ]]; then
    kill "$PARSE_PID" >/dev/null 2>&1 || true
    wait "$PARSE_PID" >/dev/null 2>&1 || true
    PARSE_PID=""
  fi
}

wait_for_port() {
  local host="$1"
  local port="$2"
  local tries="${3:-40}"
  local i
  for i in $(seq 1 "$tries"); do
    if nc -z "$host" "$port" >/dev/null 2>&1; then
      return 0
    fi
    sleep 1
  done
  return 1
}

start_parse() {
  local log_file="$1"
  shift
  stop_parse
  pushd "$ROOT_DIR" >/dev/null
  env "$@" ./parse >"$log_file" 2>&1 &
  PARSE_PID=$!
  popd >/dev/null
  if ! wait_for_port 127.0.0.1 2000 40; then
    return 1
  fi
  return 0
}

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

# SECNET-001: default bind address should remain loopback when MUD_BIND_ADDR is unset.
if start_parse "$RUN_DIR/secnet_001_parse.log" MUD_LIB="$RUNTIME_MUDLIB"; then
  if lsof -nP -a -p "$PARSE_PID" -iTCP -sTCP:LISTEN >"$RUN_DIR/secnet_001_lsof.txt" 2>&1; then
    if grep -Eq '127\.0\.0\.1:2000' "$RUN_DIR/secnet_001_lsof.txt"; then
      record_pass "SECNET-001" "default listener bind remained on loopback"
    else
      record_fail "SECNET-001" "listener did not appear loopback-bound (see secnet_001_lsof.txt)"
    fi
  else
    record_fail "SECNET-001" "failed to inspect listener socket via lsof"
  fi
else
  record_fail "SECNET-001" "parse failed to start without explicit MUD_BIND_ADDR"
fi
stop_parse

# SECNET-002: malformed ACCESS.DENY should not crash connection admission path.
cat >"$RUNTIME_MUDLIB/ACCESS.DENY" <<'EOF'
# malformed and edge rows for parser hardening regression
:
:::
not_an_ip:host:comment
127.0.0.1
127.0.0.1:localhost
127.0.0.1:localhost:allow_local_probe
EOF

if start_parse "$RUN_DIR/secnet_002_parse.log" MUD_BIND_ADDR=127.0.0.1 MUD_LIB="$RUNTIME_MUDLIB"; then
  i=0
  ok=1
  while [[ "$i" -lt 6 ]]; do
    i=$((i + 1))
    (printf 'zzzzzzzzzz\n' ; sleep 0.2) | nc 127.0.0.1 2000 >"$RUN_DIR/secnet_002_conn_${i}.log" 2>&1 || true
    if ! kill -0 "$PARSE_PID" >/dev/null 2>&1; then
      ok=0
      break
    fi
  done
  if [[ "$ok" -eq 1 ]]; then
    record_pass "SECNET-002" "malformed ACCESS.DENY did not crash admission path under repeated connects"
  else
    record_fail "SECNET-002" "parse exited during malformed ACCESS.DENY connection probes"
  fi
else
  record_fail "SECNET-002" "parse failed to start for ACCESS.DENY regression probe"
fi
stop_parse

# SECNET-003: large newline-heavy payload should not crash parser/input buffering.
if start_parse "$RUN_DIR/secnet_003_parse.log" MUD_BIND_ADDR=127.0.0.1 MUD_LIB="$RUNTIME_MUDLIB"; then
  {
    printf 'tester\n'
    python3 - <<'PY'
print(("A\n" * 7000), end="")
PY
  } | nc 127.0.0.1 2000 >"$RUN_DIR/secnet_003_payload.log" 2>&1 || true

  sleep 1
  if kill -0 "$PARSE_PID" >/dev/null 2>&1 && nc -z 127.0.0.1 2000 >/dev/null 2>&1; then
    record_pass "SECNET-003" "newline-heavy payload did not terminate driver listener"
  else
    record_fail "SECNET-003" "driver exited or listener dropped after newline-heavy payload"
  fi
else
  record_fail "SECNET-003" "parse failed to start for newline expansion regression probe"
fi
stop_parse

# SECNET-004: unresolved-hostname startup hard-fail regression is environment-level and not deterministic.
record_skip "SECNET-004" "hostname-resolution failure path is not deterministic in this harness; keep validated by code review/targeted env tests"

cat >"$RUN_DIR/SUMMARY.txt" <<EOF
Driver security suite completed.

Run ID: $RUN_ID
Run dir: $RUN_DIR_REL

Totals:
- pass: $PASS_COUNT
- fail: $FAIL_COUNT
- skip: $SKIP_COUNT

Artifacts:
- make_parse.log
- RESULTS.log
- secnet_001_parse.log
- secnet_001_lsof.txt
- secnet_002_parse.log
- secnet_002_conn_*.log
- secnet_003_parse.log
- secnet_003_payload.log
EOF

echo "==> Done: $RUN_DIR_REL"
if [[ "$FAIL_COUNT" -ne 0 ]]; then
  exit 2
fi
