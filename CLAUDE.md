# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

> **新场景 / 模态 / 菜单 / 设置的按键设计前，请先读 [`BUTTONS.md`](BUTTONS.md)。**

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

All hardware services are behind interfaces in `include/platform/interfaces/`. `IPlatform` is the composed facade that provides `IDisplay`, `IInput`, `IPower`, `ITime`, `IStorage`, `IAssetProvider`, and `write_audio_samples()`. Three platform implementations exist:

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

## Asset Pipeline

二进制资产（精灵图、关卡数据、音效）通过编译时管线转为 C++ `const` 数组，直接链接进固件。

资产和工具已迁移至 `software/game-core/` 内，使 game-core 自包含。外部消费者只需拉取 game-core 即可获得完整资产管线。

### 目录约定

```
software/game-core/
  assets/              ← 原始资产文件
    sprites/           ← 精灵数据 (.raw / .bin / 任意格式)
    levels/            ← 关卡数据
  tools/               ← 转换脚本
    asset_convert.py   ← 二进制 → C++ 数组
  src/assets/generated/   ← 预生成的 C++ 文件（提交到 git，供 CCS 直接使用）
  cmake/AssetHelpers.cmake  ← CMake 函数 add_assets()
```

### 注册资产

在平台 CMakeLists 中调用 `add_assets()`：

```cmake
# software/host-sim/sdl/CMakeLists.txt
set(GAME_CORE_ROOT "${HOST_SIM_ROOT_DIR}/../game-core")
include(${GAME_CORE_ROOT}/cmake/AssetHelpers.cmake)

add_assets(TARGET host-sim-sdl
    ASSETS
        SPRITE_ENEMY    1    ${GAME_CORE_ROOT}/assets/sprites/enemy.raw
        MAZE_01         2    ${GAME_CORE_ROOT}/assets/levels/maze_01.map
)
```

每个资产三元组 = `名称 数字ID 文件路径`。名称将成为 C++ 常量名，ID 用于运行时查找。

### 构建时自动生成

| 生成文件 | 说明 |
|---|---|
| `_assets/<NAME>.cpp` | `extern "C" alignas(4) const unsigned char` 数组 |
| `_assets/AssetId.h` | `namespace handheld::asset { constexpr uint16_t NAME = ID; }` |
| `_assets/asset_registry.cpp` | `extern const AssetEntry builtin_assets[]` 汇总表 |

`<NAME>.cpp` 按需增量编译，只重做改过的资产。`alignas(4)` 保证 ARM 访问安全。

### 在 C++ 中使用

```cpp
#include "AssetId.h"  // 由 add_assets() 生成

void MyScreen::render(IPlatform& platform, IScreenHost&) {
    const void* data;
    uint32_t size;
    if (platform.assets().get(handheld::asset::SPRITE_ENEMY, data, size)) {
        auto* pixels = static_cast<const Color*>(data);
        display.draw_bitmap({x, y}, pixels, 16, 16);
    }
}
```

### 背后工具

- `software/game-core/tools/asset_convert.py` — 读取任意二进制文件，补齐到 4 字节，输出 `extern "C"` C++ 数组
- `software/game-core/cmake/AssetHelpers.cmake` — CMake 函数 `add_assets()`，驱动转换 + 文件生成

## Audio

音效以 `inline constexpr Tone` 数组形式定义在 `software/game-core/include/core/audio/Sounds.h` 中。

`AudioEngine`（`core/audio/AudioEngine.h`）是一个 4 通道软件正弦波合成器：通道 0 用于 BGM（循环），通道 1-3 用于一次性 SFX。输出 44100Hz S16LE 单声道 PCM。

平台只需实现 `IPlatform::write_audio_samples(const int16_t* data, size_t count)` 将 PCM 数据送入硬件。

### 在游戏中使用

```cpp
#include "core/audio/Sounds.h"

void MyScreen::enter(IPlatform& platform, IScreenHost& host) {
    // 设置循环 BGM
    host.audio().set_bgm(sounds::BGM_MENU, sounds::BGM_MENU_COUNT);
}

void MyScreen::update(IPlatform& platform, IScreenHost& host) {
    // 播放一次性 SFX
    host.audio().play_sfx(sounds::SFX_SELECT, sounds::SFX_SELECT_COUNT);
}
```
