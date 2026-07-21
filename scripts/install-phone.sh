#!/usr/bin/env bash

set -euo pipefail

source "$(dirname "$0")/load-env.sh"

ip="${IP:-}"
install_args=()
ip_set_from_arg=0

while (($#)); do
  case "$1" in
    --)
      shift
      install_args+=("$@")
      break
      ;;
    -*)
      install_args+=("$1")
      shift
      ;;
    *)
      if [[ $ip_set_from_arg -eq 0 ]]; then
        ip="$1"
        ip_set_from_arg=1
      else
        install_args+=("$1")
      fi
      shift
      ;;
  esac
done

if [[ -z "$ip" ]]; then
  printf 'usage: %s [phone-ip] [-- pebble-install-args...]\n' "$0" >&2
  printf 'or set IP in environment and run: %s [pebble-install-args...]\n' "$0" >&2
  exit 1
fi

scripts/build.sh

pbw="$(ls build/*.pbw 2>/dev/null | head -n1)"
if [[ -z "$pbw" ]]; then
  echo "No .pbw found in build/ after build" >&2
  exit 1
fi

if ((${#install_args[@]})); then
  pebble install "$pbw" --phone "$ip" "${install_args[@]}"
else
  pebble install "$pbw" --phone "$ip"
fi
