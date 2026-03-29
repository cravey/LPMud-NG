#!/usr/bin/env bash

# Generate a throwaway login name accepted by test-mudlib:
# lowercase letters only, max length 11.
make_test_user() {
  local raw
  local user

  raw="$(head -c 32 /dev/urandom | od -An -tx1 | tr -d ' \n')"
  if [[ -z "$raw" ]]; then
    raw="$(printf '%s%s' "$$" "$(date +%s)")"
  fi

  user="$(printf '%s' "$raw" \
    | tr '[:upper:]' '[:lower:]' \
    | tr -d '-' \
    | tr '0-9' 'abcdefghij' \
    | tr -cd 'a-z')"

  if [[ ${#user} -lt 10 ]]; then
    user="${user}abcdefghij"
  fi

  printf '%s' "${user:0:10}"
}
