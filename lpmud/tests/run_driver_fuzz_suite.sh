#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
RUN_ID="$(date +%Y%m%d_%H%M%S)"
RUN_DIR_REL="tests/reports/driver_fuzz_suite_${RUN_ID}"
RUN_DIR="$ROOT_DIR/$RUN_DIR_REL"

ITERATIONS="${FUZZ_ITERATIONS:-250}"
SEEDS_CSV="${FUZZ_SEEDS:-1337,7331}"
MAX_PAYLOAD_LEN="${FUZZ_MAX_PAYLOAD_LEN:-2048}"
PHASE_TIMEOUT="${FUZZ_PHASE_TIMEOUT:-20}"
CMD_TIMEOUT="${FUZZ_COMMAND_TIMEOUT:-0.9}"

RUNTIME_ROOT="$(mktemp -d "${TMPDIR:-/tmp}/lpmud-driver-fuzz.XXXXXX")"
RUNTIME_MUDLIB="$RUNTIME_ROOT/mudlib"
PARSE_PID=""
ANY_FAIL=0

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

require_cmd() {
  local cmd="$1"
  if ! command -v "$cmd" >/dev/null 2>&1; then
    echo "missing required command: $cmd" >&2
    exit 1
  fi
}

require_cmd python3
require_cmd make
require_cmd nc
require_cmd lsof

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

IFS=',' read -r -a SEEDS <<< "$SEEDS_CSV"

for seed in "${SEEDS[@]}"; do
  seed_trimmed="$(echo "$seed" | tr -d '[:space:]')"
  if [[ -z "$seed_trimmed" ]]; then
    continue
  fi
  fuzz_user="$(echo "fuzz${seed_trimmed}" | tr '0-9' 'abcdefghij' | tr -cd 'a-z' | cut -c1-10)"
  if [[ -z "$fuzz_user" ]]; then
    fuzz_user="fuzzer"
  fi

  echo "==> Fuzzing seed=$seed_trimmed iterations=$ITERATIONS"
  set +e
  python3 "$ROOT_DIR/tests/fuzz_driver_surface.py" \
    --host 127.0.0.1 \
    --port 2000 \
    --user "$fuzz_user" \
    --seed "$seed_trimmed" \
    --iterations "$ITERATIONS" \
    --max-payload-len "$MAX_PAYLOAD_LEN" \
    --phase-timeout "$PHASE_TIMEOUT" \
    --command-timeout "$CMD_TIMEOUT" \
    --output-json "$RUN_DIR/fuzz_seed_${seed_trimmed}.json" \
    --transcript "$RUN_DIR/fuzz_seed_${seed_trimmed}.log"
  FUZZ_EXIT=$?
  set -e

  if [[ "$FUZZ_EXIT" -ne 0 ]]; then
    ANY_FAIL=1
  fi

  if ! kill -0 "$PARSE_PID" >/dev/null 2>&1; then
    echo "parse exited during fuzz seed=$seed_trimmed" >>"$RUN_DIR/fuzz_failures.log"
    ANY_FAIL=1
    break
  fi
done

TOTAL_REPORTS="$(find "$RUN_DIR" -maxdepth 1 -name 'fuzz_seed_*.json' | wc -l | tr -d ' ')"
CRASH_SUSPECTED="$( (grep -n '"crash_suspected": true' "$RUN_DIR"/fuzz_seed_*.json 2>/dev/null || true) | wc -l | tr -d ' ')"

cat >"$RUN_DIR/SUMMARY.txt" <<EOF
Driver fuzz suite completed.

Run ID: $RUN_ID
Run dir: $RUN_DIR_REL

Configuration:
- seeds: $SEEDS_CSV
- iterations_per_seed: $ITERATIONS
- max_payload_len: $MAX_PAYLOAD_LEN

Results:
- reports_written: $TOTAL_REPORTS
- crash_suspected_reports: $CRASH_SUSPECTED
- nonzero_exit: $ANY_FAIL

Artifacts:
- make_parse.log
- parse.log
- fuzz_seed_*.json
- fuzz_seed_*.log
EOF

echo "==> Done: $RUN_DIR_REL"
if [[ "$ANY_FAIL" -ne 0 || "$CRASH_SUSPECTED" -ne 0 ]]; then
  exit 2
fi
