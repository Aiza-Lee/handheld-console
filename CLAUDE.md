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

## Asset Pipeline

二进制资产（精灵图、关卡数据、音效）通过编译时管线转为 C++ `const` 数组，直接链接进固件。

资产和工具已迁移至 `software/game-core/` 内，使 game-core 自包含。外部消费者只需拉取 game-core 即可获得完整资产管线。

### 目录约定

```
software/game-core/
  assets/              ← 原始资产文件
    sprites/           ← 精灵数据 (.raw / .bin / 任意格式)
    sounds/            ← 音效 (.tone)
    levels/            ← 关卡数据
  tools/               ← 转换脚本
    asset_convert.py   ← 二进制 → C++ 数组
    audio2tone.py      ← .tone → C++ Tone 数组
    regenerate_assets.py  ← 一键再生所有预生成文件
  src/assets/generated/   ← 预生成的 C++ 文件（提交到 git，供 CCS 直接使用）
  cmake/AssetHelpers.cmake  ← CMake 函数 add_assets() / add_sounds()
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
- `software/game-core/tools/regenerate_assets.py` — 从原始资产重新生成 `src/assets/generated/` 下所有预生成文件

### 添加新资产步骤

1. 将原始数据文件放入 `software/game-core/assets/` 下对应子目录
2. 在 `sdl/CMakeLists.txt` 的 `add_assets()` 中增加一行三元组
3. 更新 `tools/regenerate_assets.py` 中的 ASSETS / SOUNDS 列表
4. 运行 `python3 tools/regenerate_assets.py`（在 game-core 目录下）
5. 重新构建（`cmake --build`），CMake 也会自动检测并生成

### CCS 构建（非 CMake）

CCS 用户无法使用 CMake 资产管线。预生成的 `.cpp` 文件已提交在 `src/assets/generated/` 中，可直接加入 CCS 工程：

- 将 `src/assets/generated/` 中所有 `.cpp` 文件加入 CCS 源文件列表
- 设置 include path 包含 `include/` 和 `src/assets/generated/`
- 修改资产后，在 game-core 目录下运行 `python3 tools/regenerate_assets.py` 再生所有文件

## Audio Tone Pipeline

音效以 `.tone` 格式编写，用 `audio2tone.py` 转为 C++ `Tone` 数组。

### 编写音效

`software/game-core/assets/sounds/` 目录下创建 `.tone` 文件，例如 `pickup.tone`：

```tone
# 拾取道具音效
C5 100
E5 100
G5 200
```

支持三种行格式：

| 格式 | 示例 | 含义 |
|---|---|---|
| `音符名 八度 时长` | `C4 200` | 中央 C，200ms。音符: C, C#, D, D#, E, F, F#, G, G#, A, A#, B |
| `REST 时长` | `REST 100` | 静音 100ms |
| `频率 时长` | `440 200` | 原始频率 440Hz，200ms |

### 注册音效

```cmake
# sdl/CMakeLists.txt
add_sounds(TARGET host-sim-sdl
    SOUNDS
        SCALE  ${GAME_CORE_ROOT}/assets/sounds/scale.tone
)
```

### 在游戏中使用

```cpp
// 由 add_sounds() 生成
extern "C" const handheld::Tone _sound_SCALE[];
extern "C" const uint32_t _sound_SCALE_count;

// 在 screen 的某个时机触发
platform.audio().play_sequence(_sound_SCALE, _sound_SCALE_count, false);
// 第三个参数 loop=true 可循环播放
```

### 背后工具

- `software/game-core/tools/audio2tone.py` — 读取 `.tone` 文件，计算音符频率，输出 `extern "C"` 的 Tone 数组
- `software/game-core/cmake/AssetHelpers.cmake` 中的 `add_sounds()` — CMake 封装
