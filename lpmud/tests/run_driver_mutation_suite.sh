#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

python3 "$ROOT_DIR/tests/run_driver_mutation_suite.py" \
  --source-root "$ROOT_DIR" \
  "$@"
