# Task runner for the infowatch Pebble project.
#
# Replaces the old mise tasks. Requires the Pebble toolchain on PATH:
#   - arm-none-eabi-gcc + binutils + newlib (apt)
#   - pebble-tool (pipx install pebble-tool==5.0.38)
#   - Pebble SDK from pebble-sdk-version (pebble sdk install <version>)
# In the remote/web env these are provisioned at startup. See CLAUDE.md.

.PHONY: build rebuild clean install install-phone install-cloud install-emulator emu kill-emulator kill

# Build PBW (activates pinned SDK, prepares fixture, runs pebble build)
build:
	scripts/build.sh

# Clean build artifacts
clean:
	pebble clean

# Clean and rebuild PBW
rebuild:
	pebble clean && scripts/build.sh

# Build and install on phone (alias: install)
install-phone:
	scripts/install-phone.sh
install: install-phone

# Build and install on CloudPebble (alias: cloud)
install-cloud:
	scripts/install-cloud.sh
cloud: install-cloud

# Build and install on emulator (default emulator: basalt) (alias: emu)
install-emulator:
	pebble wipe && scripts/install-emulator.sh
emu: install-emulator

# Stop running emulator and phone simulator (alias: kill)
kill-emulator:
	pebble kill
kill: kill-emulator
