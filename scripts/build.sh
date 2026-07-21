#!/usr/bin/env bash

set -euo pipefail

if [[ "${1:-}" == "--" ]]; then
  shift
fi

scripts/ensure-pebble-sdk.sh
node scripts/prepare-fixture.js
pebble build "$@"
