# host-sim

`host-sim` 保留为无图形 smoke target，用来快速验证 `game-core` 的单帧 scene runtime。

`host-sim-sdl` 是可选的 SDL3 宿主模拟器，只有在仓库根目录存在 `third_party/SDL` 子模块时才会构建。

SDL 宿主实现集中在 `software/host-sim/sdl/`，根目录只保留构建文件和无图形 smoke 入口。

推荐把 SDL3 放在固定位置：

```bash
git submodule add https://github.com/libsdl-org/SDL.git third_party/SDL
git submodule update --init --recursive
```

然后在 `software/host-sim` 下配置和构建：

```bash
cmake --preset clang-debug
cmake --build --preset clang-debug-build
./build/clang-debug/host-sim-sdl
```

当前按键映射：

- 方向键1: `W/S/A/D`
- 方向键2: `UP/DOWN/LEFT/RIGHT`
- `J`: `A`
- `K`: `B`
- `Enter`: `START`
- `Right Shift`: `SELECT`
- `Esc`: 退出模拟器
