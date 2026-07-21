#!/usr/bin/env bash

set -euo pipefail

source "$(dirname "$0")/load-env.sh"

if [[ "${1:-}" == "--" ]]; then
  shift
fi

scripts/build.sh

pbw="$(ls build/*.pbw 2>/dev/null | head -n1)"
if [[ -z "$pbw" ]]; then
  echo "No .pbw found in build/ after build" >&2
  exit 1
fi

pebble install "$pbw" --cloudpebble "$@"
