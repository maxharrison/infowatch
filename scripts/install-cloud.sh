#!/usr/bin/env bash

set -euo pipefail

if [[ "${1:-}" == "--" ]]; then
  shift
fi

mise run build
pebble install "build/forecaswatch2.pbw" --cloudpebble "$@"
