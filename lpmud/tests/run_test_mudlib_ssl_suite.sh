#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
TEST_DIR="$ROOT_DIR/tests"
source "$TEST_DIR/test_user.sh"
RUN_ID="$(date +%Y%m%d_%H%M%S)"
RUN_DIR_REL="tests/reports/test_mudlib_ssl_suite_${RUN_ID}"
RUN_DIR="$ROOT_DIR/$RUN_DIR_REL"
CLEAN_LOG="$(mktemp "${TMPDIR:-/tmp}/lpmud-make-clean-ssl.XXXXXX.log")"

TEST_USER="$(make_test_user)"

RUNTIME_ROOT="$(mktemp -d "${TMPDIR:-/tmp}/lpmud-test-mudlib-ssl.XXXXXX")"
RUNTIME_MUDLIB="$RUNTIME_ROOT/mudlib"
TLS_DIR="$RUN_DIR/tls"
CERT_FILE="$TLS_DIR/server.crt"
KEY_FILE="$TLS_DIR/server.key"
PARSE_PID=""

cleanup() {
  set +e
  if [[ -n "$PARSE_PID" ]]; then
    kill "$PARSE_PID" >/dev/null 2>&1 || true
    wait "$PARSE_PID" >/dev/null 2>&1 || true
  fi
}
trap cleanup EXIT

echo "==> Building parse with SSL support"
if ! make -C "$ROOT_DIR" clean >"$CLEAN_LOG" 2>&1; then
  echo "SSL pre-clean failed." >&2
  tail -n 80 "$CLEAN_LOG" >&2 || true
  rm -f "$CLEAN_LOG"
  exit 1
fi
mkdir -p "$RUN_DIR"
cp "$CLEAN_LOG" "$RUN_DIR/make_clean_ssl.log"
rm -f "$CLEAN_LOG"
if ! make -C "$ROOT_DIR" parse USE_SSL=1 >"$RUN_DIR/make_parse_ssl.log" 2>&1; then
  echo "SSL build failed. OpenSSL development headers/libraries are required." >&2
  tail -n 120 "$RUN_DIR/make_parse_ssl.log" >&2 || true
  exit 1
fi

echo "==> Preparing isolated test mudlib runtime"
cp -R "$ROOT_DIR/test-mudlib" "$RUNTIME_MUDLIB"

echo "==> Generating ephemeral self-signed certificate"
mkdir -p "$TLS_DIR"
openssl req -x509 -newkey rsa:2048 -nodes \
  -keyout "$KEY_FILE" \
  -out "$CERT_FILE" \
  -days 1 \
  -subj "/CN=localhost" \
  >"$RUN_DIR/openssl_req.log" 2>&1

echo "==> Ensuring localhost:2000 is free"
STALE_PIDS="$(lsof -t -nP -iTCP:2000 -sTCP:LISTEN 2>/dev/null || true)"
if [[ -n "$STALE_PIDS" ]]; then
  kill $STALE_PIDS >/dev/null 2>&1 || true
  sleep 1
fi

echo "==> Starting parse with test-mudlib + SSL"
pushd "$ROOT_DIR" >/dev/null
MUD_SSL=1 \
MUD_SSL_CERT_FILE="$CERT_FILE" \
MUD_SSL_KEY_FILE="$KEY_FILE" \
MUD_BIND_ADDR="127.0.0.1" \
MUD_LIB="$RUNTIME_MUDLIB" \
./parse >"$RUN_DIR/parse.log" 2>&1 &
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

echo "==> Running engine LPC suite through openssl s_client"
expect "$TEST_DIR/test_mudlib_suite.exp" \
  --host 127.0.0.1 \
  --port 2000 \
  --user "$TEST_USER" \
  --client openssl \
  --sni localhost \
  --transcript "$RUN_DIR/session.log" \
  >"$RUN_DIR/expect.log" 2>&1

SUMMARY_LINE="$(grep -n 'TEST_SUMMARY total=' "$RUN_DIR/session.log" | tail -n 1 || true)"
RESULT_LINE="$(grep -n 'SUITE_RESULT total=' "$RUN_DIR/expect.log" | tail -n 1 || true)"

cat >"$RUN_DIR/SUMMARY.txt" <<EOF
Test mudlib SSL suite completed.

Run ID: $RUN_ID
Run dir: $RUN_DIR_REL
Test user: $TEST_USER

Summary line:
$SUMMARY_LINE

Expect result:
$RESULT_LINE

Artifacts:
- make_clean_ssl.log
- make_parse_ssl.log
- openssl_req.log
- parse.log
- expect.log
- session.log
EOF

echo "==> Done: $RUN_DIR_REL"
