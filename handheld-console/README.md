# Handheld Console Firmware

This directory is the CCS/TI firmware project for the MSPM0G3507 target board.

## Repository Layout

- `app/main.cpp`: firmware entry point and top-level wiring
- `board.syscfg`: clock and peripheral configuration source for SysConfig
- `platform/mspm0/`: MSPM0-specific platform adapters
- `drivers/`: low-level hardware drivers
- `targetConfigs/`: CCS target configuration
- `../software/game-core/`: shared runtime, interfaces, null platform, and game logic
- `../software/host-sim/`: host-side CMake build for logic smoke tests in VS Code

## Development Flow

- Open only `handheld-console/` in CCS.
- Use the linked `game-core` folder inside the CCS project for shared logic sources.
- `game-core/tests/` is excluded from the CCS firmware build; keep host-side smoke tests under `software/host-sim/`.
- Use `software/host-sim/` from VS Code to build and run headless logic checks without TI tools.

## Current Status

- `app/main.cpp` remains the only firmware composition entry point.
- Portable game logic lives in `../software/game-core/include` and `../software/game-core/src`.
- `platform/null/NullPlatform.h` remains available for host-side simulation and tests.
- `platform/mspm0/` is reserved for the real hardware adapter implementation.
