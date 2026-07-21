#!/usr/bin/env bash

# Load variables from the repo's .env into the environment.
#
# mise used to do this automatically ([env] _.file = ".env"). Since mise was
# removed, the build/install scripts source this helper so IP, FIXTURE,
# ENABLE_MEMORY_LOGGING, etc. still take effect.
#
# Usage (from a script whose cwd is the repo root):
#   source scripts/load-env.sh
#
# Lines are `KEY=value`; blank lines and `#` comments are ignored. Existing
# environment values win, so `FIXTURE=foo scripts/build.sh` overrides .env.

_load_env_file() {
  local env_file="${1:-.env}"
  [[ -f "$env_file" ]] || return 0

  local line key value
  while IFS= read -r line || [[ -n "$line" ]]; do
    # strip leading/trailing whitespace
    line="${line#"${line%%[![:space:]]*}"}"
    line="${line%"${line##*[![:space:]]}"}"
    [[ -z "$line" || "$line" == \#* ]] && continue
    [[ "$line" != *=* ]] && continue

    key="${line%%=*}"
    value="${line#*=}"
    key="${key%"${key##*[![:space:]]}"}"   # rtrim key
    key="${key#export }"                     # tolerate `export KEY=...`
    key="${key#"${key%%[![:space:]]*}"}"     # ltrim key

    # Don't clobber values already set in the environment.
    [[ -n "${!key:-}" ]] && continue

    export "$key=$value"
  done < "$env_file"
}

_load_env_file ".env"
