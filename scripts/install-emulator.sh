#!/usr/bin/env bash

set -euo pipefail

emulator="${PEBBLE_EMULATOR:-basalt}"
install_args=()

if [[ "${1:-}" == "--" ]]; then
  shift
fi

while (($#)); do
  case "$1" in
    --emulator)
      if [[ -z "${2:-}" ]]; then
        echo "Missing value for --emulator" >&2
        exit 1
      fi
      emulator="$2"
      shift 2
      ;;
    --emulator=*)
      emulator="${1#*=}"
      shift
      ;;
    aplite|basalt|chalk|diorite|emery|flint|gabbro)
      emulator="$1"
      shift
      ;;
    *)
      install_args+=("$1")
      shift
      ;;
  esac
done

mise run build

if ((${#install_args[@]})); then
  pebble install build/forecaswatch2.pbw --emulator "$emulator" "${install_args[@]}"
else
  pebble install build/forecaswatch2.pbw --emulator "$emulator"
fi
