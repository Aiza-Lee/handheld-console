# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Repository Overview

A handheld game console project with three layers:

- **`handheld-console/`** — CCS/TI firmware project targeting MSPM0G3507. Entry point at `app/main.cpp`. Platform adapters live under `platform/mspm0/` (currently stubbed with NullPlatform).
- **`software/game-core/`** — Portable, header-heavy C++17 runtime library shared between firmware and host-side tools. Contains screen lifecycle (stack-based), platform interfaces, graphics primitives, and bitmap font rendering.
- **`software/host-sim/`** — CMake-based host-side build for running game-core logic without TI hardware. Two targets: `host-sim` (headless smoke test) and `host-sim-sdl` (SDL3 windowed simulator, only builds if `third_party/SDL` submodule exists).

## Build Commands (from `software/host-sim/`)

```bash
# Configure
cmake --preset clang-debug

# Build all targets (host-sim, host-sim-sdl if SDL present, tests)
cmake --build --preset clang-debug-build

# Run headless smoke
./build/clang-debug/host-sim

# Run SDL simulator
./build/clang-debug/host-sim-sdl

# Run all CTest tests
ctest --preset clang-debug-test

# Run a single test by name
ctest -R game-core-smoke
```

## Architecture

### Platform Abstraction

All hardware services are behind interfaces in `include/platform/interfaces/`. `IPlatform` is the composed facade that provides `IDisplay`, `IInput`, `IAudio`, `IPower`, `ITime`, and `IStorage`. Three platform implementations exist:

| Implementation | Location | Use |
|---|---|---|
| `FakePlatform` | `game-core/tests/support/FakePlatform.h` | Test doubles with counters and settable state |
| `SdlPlatform` | `host-sim/sdl/SdlPlatform.h` | SDL3 windowed simulator with framebuffer, keyboard-to-button mapping |
| NullPlatform | Referenced by firmware `app/main.cpp` | Hardware placeholder |

### Screen Lifecycle (Stack-Based)

`ScreenRunner` drives the game loop. Screens are organized in a stack — only the top screen receives `update()` and `render()`. On each `tick()`:
1. Apply pending stack operation (switch/push/pop) — see `IScreenHost`
2. If newly entered screen, call `enter()`
3. `IInput::poll()` → top `GameScreen::update()` → top `GameScreen::render()` → `IDisplay::present()`
4. Apply pending stack operation again (for requests issued during `update()`)

Screens request transitions via `IScreenHost`:
- `switch_to(ScreenType)` — replace entire stack (menu ↔ game)
- `push_screen(ScreenType)` — push overlay, lower screen receives `suspend()`
- `pop_screen()` — remove overlay, lower screen receives `resume()`

`ScreenType` is an enum (`MENU`, `PLAYGROUND`). `DefaultScreenFactory` maps types to concrete screen classes.

### Input Model

`ButtonBits` is a bitmask enum with 8 buttons (UP/DOWN/LEFT/RIGHT/A/B/START/SELECT). `ButtonState` wraps a `uint8_t` with `test()`, `set()`, `any()`, `none()`. `IInput` provides convenience methods: `is_down()` (current frame), `was_pressed()` (edge: now down, was up), `was_released()` (edge: now up, was down).

### Graphics

- `Color` — RGB565 `uint16_t` enum with named constants and `rgb565()` factory
- `Geometry` — `Point`, `Size`, `Rect` value types
- `Font` — `BitmapFont` struct with `glyph_for()` lookup. Two constexpr instances: `BASIC_FONT_5X7` (5x7 glyphs) and `COMPACT_FONT_3X5` (3x5 glyphs, packed into `uint16_t` for size)
- `TextRenderer` — static methods only: `measure_text()`, `draw_text()`, `draw_glyph()`. Supports multi-line via `\n` and integer scaling
- `IDisplay` has non-virtual drawing helpers in the header: `draw_h_line`, `draw_v_line`, `draw_line` (Bresenham), `draw_rect`, `fill_rect`, `draw_bitmap`

## Style

- C++17, compiled with Clang, CMake 3.28+
- Clang-format config at repo root: 4-space tabs, K&R braces, 120 column limit, pointer alignment left
- Namespace: `handheld`
- Non-copyable, non-movable base classes (interface pattern)
- Header guards: `#ifndef` style
- Comments in Chinese

## Testing

Smoke tests are standalone executables (no test framework), each with its own `main()` using `assert()`. Tests link `game_core` and use `FakePlatform` with configurable `FakeInput` (call `fake_input().set_button()` before `tick()`). Each test is registered with CTest via `add_game_core_smoke_test()` in `tests/CMakeLists.txt`.
