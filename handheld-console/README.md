# Handheld Console Skeleton

This project is a clean MSPM0G3507 C++ skeleton for a small handheld game device.

## Directory Layout

- `app/`: application entry point and top-level wiring
- `platform/interfaces/`: hardware abstraction interfaces used by game logic
- `platform/mspm0/`: MSPM0-specific implementations
- `drivers/`: low-level drivers for LCD, buttons, buzzer, and battery sensing
- `core/`: reusable runtime and shared types
- `games/`: menu and game modules
- `assets/`: fonts and sprites

## Current Status

- `app/main.cpp` boots the board and enters the app skeleton
- `app/App.h` and `app/App.cpp` hold the top-level application shell
- `board.syscfg` keeps the board clock configuration
- interface headers are in place for display, input, audio, and power
- a placeholder `MenuGame` is available as the first scene

## Next Steps

1. Configure the real board pins and peripherals in `board.syscfg`
2. Implement `platform/mspm0/` adapters for the target hardware
3. Add LCD drawing primitives and button scanning
4. Replace the placeholder menu with a real scene manager
