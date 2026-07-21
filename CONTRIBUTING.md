# Contributing

## Building

Prerequisites: [Node.js](https://nodejs.org/en/) and the Pebble toolchain.

Install the toolchain (Debian/Ubuntu):

```bash
# ARM toolchain + pipx (the Pebble SDK uses the system arm-none-eabi-gcc)
sudo apt-get install -y gcc-arm-none-eabi binutils-arm-none-eabi libnewlib-arm-none-eabi pipx

# pebble-tool — install via pipx, NOT system pip
# (system pip fails building the pyqrcode dep against Debian's patched setuptools)
export PATH="$HOME/.local/bin:$PATH"
pipx install pebble-tool==5.0.38

# Pre-download the pinned SDK (from pebble-sdk-version)
yes | pebble sdk install "$(cat pebble-sdk-version)"
```

Then build:

```bash
# Install JS dependencies
npm install

# Build
make build
```

This builds the project with the Pebble SDK version pinned in `pebble-sdk-version` and provisioned by the repo scripts. The `.pbw` output can be found in the `build` directory.

## Project manifest

`package.json` is a plain, committed Pebble manifest (top-level `pebble` object). It is checked in so the project can be imported and built directly by [CloudPebble](https://cloudpebble.net) — add this GitHub repo as a source and it builds without any local tooling.

`src/pkjs/active-fixture.generated.js` (default `module.exports = null`) is committed for the same reason, since `src/pkjs/index.js` requires it unconditionally. Local fixture builds overwrite it; don't commit that regenerated version. Restore the committed default with `FIXTURE= node scripts/prepare-fixture.js`.

If you want the extra Pebble heap debug logs, set `ENABLE_MEMORY_LOGGING=1` in your `.env` before building or installing.

For deterministic emulator UI, set `FIXTURE=<name>` in `.env` before building or installing. Fixture files live in `fixtures/<name>.json` and define the watch facts and weather payload used by local builds.

## Install tasks

You can run Pebble CLI commands directly, or use install tasks that build and install in one command:

```bash
# Install on phone (set IP in .env, or pass it explicitly)
cp .env.example .env   # then edit .env and set IP=<PHONE_IP>
make install-phone
scripts/install-phone.sh <PHONE_IP>

# Pass through pebble install flags
scripts/install-phone.sh --logs

# Install via CloudPebble
make install-cloud

# Install to emulator (default emulator: basalt)
make install-emulator
scripts/install-emulator.sh aplite            # choose platform
PEBBLE_EMULATOR=aplite scripts/install-emulator.sh
scripts/install-emulator.sh --logs            # pass through pebble flags

# Stop running emulator and phone simulator
make kill-emulator
```

## Config

Local dev config has two layers:

- Use `fixtures/*.json` for committed, deterministic UI state: watch facts, Clay render settings, weather payloads, and other data that should make emulator screenshots reproducible.
- Use `src/pkjs/dev-config.js` for uncommitted behavior testing, including preloaded Clay settings when you are exercising real app flows instead of deterministic fixture UI.

When a fixture is active, prefer `claySettings` in the fixture for render-affecting Clay values. `dev-config.js` remains useful for local-only behavior switches and non-fixture testing.

Example:

```javascript
module.exports.owmApiKey = 'abc123';
```

### PKJS storage reset (dev)

Use this key in `src/pkjs/dev-config.js` to force a PKJS `localStorage` reset on each app boot while enabled:

- `clearPkjsStorageOnBoot = true`

Example:

```javascript
module.exports.clearPkjsStorageOnBoot = true;
```

This is local-only dev behavior and is not written into Clay settings.

### Fixtures (emulator/dev)

Set `FIXTURE=<name>` in `.env` to load deterministic app-state data from `fixtures/<name>.json`.
Fixtures currently support:

- `watch.now`: local date/time fields used for C-rendered time/date UI.
- `watch.battery.percent`: battery level used for C-rendered battery UI, 0-100.
- `watch.battery.charging`: optional battery charging/plugged state, `true` or `false`.
- `watchSettings.timeFormat`: watch-level time display preference, `"12h"` or `"24h"`.
- `claySettings`: Clay-compatible settings keyed by `messageKey`, such as `"firstWeek": "curr"`. Color settings use Pebble SDK color constants like `"GColorOrange"` from the Rebble color definitions: https://developer.rebble.io/docs/c/Graphics/Graphics_Types/Color_Definitions/
- `weather.city`: weather status city label.
- `weather.currentTemp`: current temperature in Fahrenheit.
- `weather.startHour`: local hour for the first forecast entry; fixture prep converts it to the runtime forecast timestamp.
- `weather.temps`: hourly Fahrenheit forecast values.
- `weather.precipPct`: hourly precipitation percentages, 0-100.
- `weather.sunEvents`: the next two sun events as local fields, e.g. `{ "type": "sunset", "dayOffset": 0, "hour": 20, "minute": 10 }`.

Minimal `.env`:

```bash
FIXTURE=readme
```

Fixture data is tracked in git inside `fixtures/`.

### Fixture emulator installs

Fixture time and battery state are compiled into the app through `watch.now` and `watch.battery`; `scripts/install-emulator.sh` does not call Pebble's emulator setting controls.

```bash
scripts/install-emulator.sh --logs
```

## Upgrading pebble-tool

This project pins `pebble-tool` to an exact version (installed via pipx; see the build prerequisites above).

To bump the pinned version, install the new version and update the references in `CONTRIBUTING.md` and `CLAUDE.md`:

```bash
pipx install --force pebble-tool==<new-version>
```
