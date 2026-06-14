# Handheld Console

基于 MSPM0G3507 的掌机项目。固件本体在仓库外（`~/workspace_ccstheia/gamepad`），本仓库承载**可移植的 C++17 运行时**和**桌面模拟器**。

## 三层结构

```
本仓库
├── software/game-core/   平台无关运行时（ScreenRunner + 13 个 Screen + 音频/图形/数学）
└── software/host-sim/    CMake 双 target：无图形 smoke + SDL3 桌面模拟器
固件（仓库外）
└── gamepad/              CCS Theia 项目，链接 game-core
```

`game-core` 不知道硬件存在；硬件适配只需实现 `IPlatform` 五个子接口（`IDisplay` / `IInput` / `IPower` / `ITime` / `write_audio_samples`）。

## 快速上手

```bash
cd software/host-sim
cmake --preset clang-debug
cmake --build --preset clang-debug-build

# 无图形烟测（验证 game-core 不崩）
./build/clang-debug/host-sim

# SDL3 桌面模拟器（需先 `git submodule add https://github.com/libsdl-org/SDL third_party/SDL`）
./build/clang-debug/host-sim-sdl

# 跑全部 smoke test
ctest --preset clang-debug-test
```

## 已实现 Screen（13 个）

菜单 / 设置 / 引导 / 开发者彩蛋
+ Snake / Pacman / Breakout / Invaders / GrowBall / Tetris / 2048 / Pong / MP3 播放器
+ Playground（5 模式按 SELECT 切换）
