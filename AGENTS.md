## Dev Iteration Flow

After you've added a feature run `mise build` to verify it builds.

If you need runtime logs, `mise install-emulator --logs` runs it in an emulator and prints logs to the terminal. The process stays alive until the emulator is closed

### Building in Claude Code on the web / remote sessions

`mise build` assumes a local Pebble SDK. In a fresh web/remote session that setup is usually **not** present:

- `mise` is not preinstalled, and the installer (`mise.run`) is blocked by the agent proxy (403), so you often can't install it on the fly.
- Even with `mise`, `pebble build` needs the ARM SDK toolchain, which is a large download from Rebble/coredevices hosts that may not be reachable.

Consequence: **you frequently cannot compile C locally.** Plan around it:

- **GitHub Actions CI is the real build check** — it compiles every target on push. Push early and read the CI logs rather than batching many unverified C changes into one push. One compile failure aborts the run before later files are checked, so fix-and-repush is normal.
- If you need a local loop, add a **SessionStart hook** that installs `pebble-tool` (pipx; `pypi.org` is allowlisted) and the SDK, and confirm the SDK actually downloads before relying on it.
- When you can't build, review C changes extra carefully against the pitfalls below — the compiler is unforgiving here.

## Pebble C build gotchas

CI compiles with `-std=c99 -Wall -Wextra -Werror`, so **a warning fails the build.** A few warnings are downgraded to non-fatal (`-Wno-error=format-truncation`, `-Wno-error=unused-function`, `-Wno-error=unused-variable`, `-Wno-error=unused-value`, and a handful of others); everything else is a hard error. Assume any warning you introduce will fail unless it's on that list.

- **Don't redefine macros from `pebble.h`.** It already defines `MINUTES_PER_DAY`, `SECONDS_PER_DAY`, `HOURS_PER_DAY`, `MINUTES_PER_HOUR`, `SECONDS_PER_MINUTE`, etc. Redefining one (even to the same value) is a `-Werror` failure. Grep `pebble.h` before adding a time/units constant. (This exact collision broke a build.)
- **Size `snprintf` buffers for the max possible output**, not the typical case — tight buffers trip `-Wformat-truncation`.
- New `.c` files under `src/c/**` are auto-globbed by `wscript`; no manual registration needed.

## Platform coverage

Target platforms are `aplite`, `basalt`, `diorite`, `emery`, `flint` — the build compiles all of them. Only **emery** has a distinct screen (200x228); the rest (including the newer **flint**) are rectangular **144x168**. Layout code branches on `#ifdef PBL_PLATFORM_EMERY`; keep the non-emery path derived from `layer_get_bounds` (screen-size-dynamic) so aplite/basalt/diorite/flint all lay out correctly from one code path.

## Adding a setting or weather field (end-to-end plumbing)

These features thread through several files. Miss one and it silently no-ops. Follow the whole chain:

**A Clay config setting** (user-facing option):
1. `src/pkjs/clay/config.js` — add the UI control (`messageKey`).
2. `src/pkjs/index.js` — add a default in `getDefaultClaySettings()` and send it in `sendClaySettings()` (as a `CLAY_*` key).
3. `package.json` — add the `CLAY_*` key to `pebble.messageKeys`.
4. `src/c/appendix/app_message.c` — read the tuple in the **clay-config branch** and populate `Config`. Read new tuples with a null-guard rather than adding them to the branch's `&&` gate, so older payloads still parse.
5. `src/c/appendix/config.h` — add the field to `Config`; set defaults in `config.c` (`config_defaults`) **and** `persist.c` (`persist_init`). `Config` is persisted as a raw blob, so **append new fields at the END** for backward compatibility with configs written before the field existed.

**A weather data field** (fetched, shown on the watch):
1. `package.json` — add the message key.
2. `src/pkjs/weather/*.js` — set it on the provider in `withProviderData` (per provider), and include it in `getPayload()` in `provider.js`. Use a sentinel (e.g. `-1`) for "not supplied" so the watch can hide it.
3. `src/c/appendix/app_message.c` — read + persist it in the **weather branch** (optional tuple; guard for missing).
4. `src/c/appendix/persist.c/.h` — add get/set and a default in `persist_init`.
5. Render it in the relevant layer.

**Fixtures** (`fixtures/*.json`, selected via `FIXTURE=<name>`): carry `claySettings` and `weather` blocks for emulator testing. Wire new weather fields through `getFixtureWeatherPayload()` in `index.js`; unknown fields pass through the `prepare-fixture.js` normalizer untouched.

## Debugging

- C: `APP_LOG(APP_LOG_LEVEL_DEBUG, "msg", args)`
- Heap probes: use `MEMORY_LOG_HEAP("tag")` for dev-only `MEM|...` logs around lifecycle and redraw checkpoints.
- JS: `console.log("msg")`

## Pebble Memory Tips

- Lazy-load bitmaps and destroy them when they are not needed to keep startup and steady-state heap usage low.
- Prefer drawing directly in an update proc over creating extra layer objects when a simple render path is enough.
- If a UI element only exists to paint pixels, keep it as light as possible instead of modeling it as a full layer.
- Avoid floating-point math and 64-bit division in watch-side C, prefer integer multiply-before-divide so linked software math helper code doesn't consume heap. See #163.

## Code Conventions

- For new JavaScript functions, add brief JSDoc (`@param`/`@returns`) annotations since this project does not use TypeScript.
- Prefer `Boolean(value)` over `!!value` in new/edited code for readability.
- When branching on `#ifdef PBL_PLATFORM_EMERY`, add a brief `emery:` comment explaining the Emery-specific behavior.
