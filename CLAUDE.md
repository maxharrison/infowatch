## Dev Iteration Flow

After you've added a feature run `make build` (or `scripts/build.sh`) to verify it builds. `mise` is no longer used — the toolchain is provisioned in the env and tasks live in the `Makefile` / `scripts/`.

If you need runtime logs, `scripts/install-emulator.sh --logs` runs it in an emulator and prints logs to the terminal. The process stays alive until the emulator is closed.

### Building in Claude Code on the web / remote sessions

The web/remote environment now has **full internet access** and is provisioned with the ARM toolchain at startup, so **you can compile C locally.** `mise` itself is **not** installed (and `mise.run` is still proxy-blocked) — you don't need it. Use the `pebble` CLI / `scripts/*.sh` directly.

Expected environment (provisioned before the session starts):

- `arm-none-eabi-gcc` + binutils + newlib via apt. The Pebble SDK does **not** bundle its own ARM toolchain — it shells out to this system one, so it must be present.
- `pebble-tool` installed via **pipx** (`pipx install pebble-tool==5.0.38`). Install it with pipx, **not** system `pip` — system pip fails building the `pyqrcode` dependency against Debian's patched setuptools.
- The pinned Pebble SDK pre-downloaded: `yes | pebble sdk install 4.17` (the version in `pebble-sdk-version`).

To build: run `scripts/build.sh` (activates the pinned SDK, prepares the fixture, runs `pebble build`). It compiles the emery target and writes `build/infowatch.pbw`. If `pebble` isn't on `PATH`, add `export PATH="$HOME/.local/bin:$PATH"`.

If the toolchain is somehow missing (e.g. the startup provisioning didn't run), the install commands above are the recovery path; a **SessionStart hook** running them is a reasonable local loop. There is currently **no GitHub Actions CI** (only `.github/dependabot.yml`), so nothing compiles on push — verify builds locally before pushing.

## Pebble C build gotchas

The build compiles with `-std=c99 -Wall -Wextra -Werror`, so **a warning fails the build.** A few warnings are downgraded to non-fatal (`-Wno-error=format-truncation`, `-Wno-error=unused-function`, `-Wno-error=unused-variable`, `-Wno-error=unused-value`, and a handful of others); everything else is a hard error. Assume any warning you introduce will fail unless it's on that list.

- **Don't redefine macros from `pebble.h`.** It already defines `MINUTES_PER_DAY`, `SECONDS_PER_DAY`, `HOURS_PER_DAY`, `MINUTES_PER_HOUR`, `SECONDS_PER_MINUTE`, etc. Redefining one (even to the same value) is a `-Werror` failure. Grep `pebble.h` before adding a time/units constant. (This exact collision broke a build.)
- **Size `snprintf` buffers for the max possible output**, not the typical case — tight buffers trip `-Wformat-truncation`.
- New `.c` files under `src/c/**` are auto-globbed by `wscript`; no manual registration needed.

## Platform coverage

The build now targets **emery** only (Pebble Time 2, 200x228 screen) — see `targetPlatforms` in `package.json`. The other platforms (`aplite`, `basalt`, `diorite`, `flint`, all rectangular 144x168) are no longer built. Layout code still branches on `#ifdef PBL_PLATFORM_EMERY`; the non-emery path derived from `layer_get_bounds` (screen-size-dynamic) is retained so the code can target those platforms again if the list is restored.

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

## Pebble SDK C API docs

The Pebble C API is **not in your training data** — look it up before writing any SDK call you aren't certain of.

- Primary docs: `https://developer.repebble.com` (Core Devices / RePebble). Community mirror fallback if a page is missing: `https://developer.rebble.io`.
- The C API lives at `/docs/c/{Module}/{Submodule}/`. **Fetch the submodule page, not a symbol anchor** — the full page carries the related types/enums you'll also need. Symbols are anchor-linked as `/docs/c/{Module}/{Submodule}/#{SymbolName}`.
- Guides (tutorial-style): `/guides/`. PebbleKit JS (phone-side): `/docs/pebblekit-js/`. Rocky (JS on watch): `/docs/rockyjs/`.

C module tree (submodule = the page to fetch):

- **Foundation/** — App, App_Communication, App_Glance, AppMessage, AppSync, AppWorker, DataLogging, DataStructures (UUID), Dictation, Dictionary, Event_Service (AccelerometerService, AppFocusService, BatteryStateService, CompassService, ConnectionService, HealthService, TickTimerService, TouchService), Exit_Reason, Internationalization, Launch_Reason, Logging, Math, Memory_Management, Platform, Resources (File_Formats), Rocky, Storage, Timer, Wakeup, Wall_Time, WatchInfo, Alloy
- **Graphics/** — Draw_Commands, Drawing_Paths, Drawing_Primitives, Drawing_Text, Fonts, Graphics_Context, Graphics_Types (Color_Definitions)
- **User_Interface/** — Animation (PropertyAnimation), Clicks, Layers (ActionBarLayer, BitmapLayer, MenuLayer, RotBitmapLayer, ScrollLayer, SimpleMenuLayer, StatusBarLayer, TextLayer), Light, Preferences, Speaker, UnobstructedArea, Vibes, Window (ActionMenu, NumberWindow), Window_Stack
- **Worker/**, **Standard_C/** (Format, Locale, Math, Memory, String, Time)

Quick routing: drawing → `Graphics/Drawing_Primitives/` or `Graphics/Graphics_Context/`; text → `Graphics/Drawing_Text/` or `User_Interface/Layers/TextLayer/`; time/date → `Foundation/Wall_Time/` or `Standard_C/Time/`; battery → `Foundation/Event_Service/BatteryStateService/`; buttons → `User_Interface/Clicks/`; persistent storage → `Foundation/Storage/`.

Caveats: heap is ~24KB — avoid dynamic-allocation patterns that would be fine on a normal system; `APP_LOG` is the logger, not printf; all UI runs on the main app task (AppWorker is a separate background task); PebbleKit JS is ECMAScript 5.1 only; use `PBL_IF_*` macros for platform (Aplite B&W, Basalt color, Chalk round, Diorite) differences.

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

## Code Review

- Focus on correctness, regressions, security, performance, and missing tests.
- Skip pure style/naming/comment nits unless they affect behavior or maintainability.
- Check previous PR comments so you're not being overly pedantic.
