#!/usr/bin/env bash
set -euo pipefail

NG_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
TEST_DIR="$NG_ROOT/tests"
RUN_ID="$(date +%Y%m%d_%H%M%S)"
RUN_DIR_REL="tests/reports/driver_diff_suite_${RUN_ID}"
RUN_DIR="$NG_ROOT/$RUN_DIR_REL"

ORIG_ROOT_DEFAULT="$NG_ROOT/../lpmud-orig/lpmud"
ORIG_ROOT="${ORIG_LPMUD_ROOT:-$ORIG_ROOT_DEFAULT}"

NG_PARSE="${NG_PARSE_BIN:-$NG_ROOT/parse}"
ORIG_PARSE="${ORIG_PARSE_BIN:-$ORIG_ROOT/parse}"

CURRENT_PARSE_PID=""

require_cmd() {
  local cmd="$1"
  if ! command -v "$cmd" >/dev/null 2>&1; then
    echo "missing required command: $cmd" >&2
    exit 1
  fi
}

cleanup() {
  set +e
  if [[ -n "$CURRENT_PARSE_PID" ]]; then
    kill "$CURRENT_PARSE_PID" >/dev/null 2>&1 || true
    wait "$CURRENT_PARSE_PID" >/dev/null 2>&1 || true
    CURRENT_PARSE_PID=""
  fi
}
trap cleanup EXIT

extract_status_map() {
  local session_log="$1"
  local out_tsv="$2"

  {
    grep -Eo '\[(PASS|FAIL)\]\[[^]]+\]' "$session_log" || true
  } | sed -E 's/\[(PASS|FAIL)\]\[([^]]+)\]/\2\t\1/' \
    | awk -F'\t' '{ status[$1]=$2 } END { for (id in status) print id "\t" status[id] }' \
    | sort >"$out_tsv"
}

run_suite_for_driver() {
  local label="$1"
  local parse_bin="$2"
  local parse_cwd="$3"

  local run_subdir="$RUN_DIR/$label"
  local runtime_root runtime_mudlib
  local summary_line expect_line expect_exit ready stale_pids

  mkdir -p "$run_subdir"

  if [[ ! -x "$parse_bin" ]]; then
    echo "parse binary is missing or not executable: $parse_bin" >&2
    exit 1
  fi

  runtime_root="$(mktemp -d "${TMPDIR:-/tmp}/lpmud-driver-diff.${label}.XXXXXX")"
  runtime_mudlib="$runtime_root/mudlib"
  cp -R "$NG_ROOT/test-mudlib" "$runtime_mudlib"

  stale_pids="$(lsof -t -nP -iTCP:2000 -sTCP:LISTEN 2>/dev/null || true)"
  if [[ -n "$stale_pids" ]]; then
    kill $stale_pids >/dev/null 2>&1 || true
    sleep 1
  fi

  pushd "$parse_cwd" >/dev/null
  MUD_BIND_ADDR="127.0.0.1" MUD_LIB="$runtime_mudlib" "$parse_bin" >"$run_subdir/parse.log" 2>&1 &
  CURRENT_PARSE_PID=$!
  popd >/dev/null

  ready=0
  for _ in $(seq 1 60); do
    if nc -z 127.0.0.1 2000 >/dev/null 2>&1; then
      ready=1
      break
    fi
    sleep 1
  done
  if [[ "$ready" -ne 1 ]]; then
    echo "$label parse did not open localhost:2000 in time" >&2
    tail -n 200 "$run_subdir/parse.log" >&2 || true
    exit 1
  fi

  set +e
  expect "$TEST_DIR/test_mudlib_suite.exp" \
    --host 127.0.0.1 \
    --port 2000 \
    --user "diff${label}" \
    --transcript "$run_subdir/session.log" \
    >"$run_subdir/expect.log" 2>&1
  expect_exit=$?
  set -e

  kill "$CURRENT_PARSE_PID" >/dev/null 2>&1 || true
  wait "$CURRENT_PARSE_PID" >/dev/null 2>&1 || true
  CURRENT_PARSE_PID=""

  summary_line="$(grep -n 'TEST_SUMMARY total=' "$run_subdir/session.log" | tail -n 1 || true)"
  expect_line="$(grep -n 'SUITE_RESULT total=' "$run_subdir/expect.log" | tail -n 1 || true)"
  extract_status_map "$run_subdir/session.log" "$run_subdir/status.tsv"

  cat >"$run_subdir/RESULT.txt" <<EOF
Driver label: $label
Parse binary: $(basename "$parse_bin")
Parse cwd label: $label
Expect exit code: $expect_exit
Summary line: $summary_line
Expect line: $expect_line
EOF
}

require_cmd grep
require_cmd sed
require_cmd awk
require_cmd expect
require_cmd nc
require_cmd lsof

mkdir -p "$RUN_DIR"

echo "==> Running differential suite: ng"
run_suite_for_driver "ng" "$NG_PARSE" "$NG_ROOT"

echo "==> Running differential suite: orig"
run_suite_for_driver "orig" "$ORIG_PARSE" "$ORIG_ROOT"

awk -F'\t' '
  NR==FNR { ng[$1]=$2; ids[$1]=1; next }
  { orig[$1]=$2; ids[$1]=1 }
  END {
    for (id in ids) {
      ngs = (id in ng) ? ng[id] : "MISSING";
      os = (id in orig) ? orig[id] : "MISSING";
      if (ngs != os)
        print id "\t" ngs "\t" os;
    }
  }
' "$RUN_DIR/ng/status.tsv" "$RUN_DIR/orig/status.tsv" | sort >"$RUN_DIR/diff.tsv"

NG_TOTAL="$(wc -l <"$RUN_DIR/ng/status.tsv" | tr -d ' ')"
ORIG_TOTAL="$(wc -l <"$RUN_DIR/orig/status.tsv" | tr -d ' ')"
DIFF_TOTAL="$(wc -l <"$RUN_DIR/diff.tsv" | tr -d ' ')"

cat >"$RUN_DIR/SUMMARY.txt" <<EOF
Driver differential suite completed.

Run ID: $RUN_ID
Run dir: $RUN_DIR_REL

Captured IDs:
- NG: $NG_TOTAL
- Orig: $ORIG_TOTAL
- Status diffs: $DIFF_TOTAL

Artifacts:
- ng/parse.log
- ng/expect.log
- ng/session.log
- ng/RESULT.txt
- ng/status.tsv
- orig/parse.log
- orig/expect.log
- orig/session.log
- orig/RESULT.txt
- orig/status.tsv
- diff.tsv
EOF

echo "==> Done: $RUN_DIR_REL"
