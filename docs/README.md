# 软件运行时流程文档

## 架构

三层结构：

- **固件**（`handheld-console/`，MSPM0G3507，本仓库外）
- **game-core**（`software/game-core/`，平台无关 C++17 运行时）—— 核心是 `ScreenRunner`，每帧 `tick()` 推进屏幕栈、输入、渲染、音频
- **host-sim**（`software/host-sim/`，SDL3 桌面模拟器）—— `ScreenRunner` 的一种平台适配

业务代码只依赖 `game-core`，对平台无感知。

## 流程图

- [`00-flow.puml`](00-flow.puml) — **整体大致流程**：启动 → 主循环 → 单帧 tick → 退出
- [`01-tick.puml`](01-tick.puml) — **单帧 `tick()` 6 阶段展开**（什么时候看：调屏幕栈切换 / 音频派发 / enter-exit 时机时）

它们**不**画：屏幕栈状态机、AudioEngine 通道、15 个屏幕跳转、IPlatform 适配细节、SdlPlatform 子对象。这些请直接读源码。

## 渲染

```bash
java -jar .tools/plantuml.jar -tpng -o out docs/*.puml
```

VS Code 用户装 PlantUML 扩展即可预览。

## 关键源码位置

- `software/game-core/src/core/runtime/ScreenRunner.cpp:138-167` — `tick()` 主体
- `software/game-core/include/core/runtime/IScreenHost.h:24-33` — 切换 API（switch_to / push / pop）
- `software/game-core/include/core/runtime/GameScreen.h:10-36` — 屏幕生命周期（enter / exit / suspend / resume）
- `software/host-sim/sdl/main.cpp:9-30` — SDL 主循环入口
