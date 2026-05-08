#ifndef CORE_RUNTIME_GAME_SCREEN_H
#define CORE_RUNTIME_GAME_SCREEN_H

#include "platform/interfaces/IPlatform.h"

namespace handheld {

class IScreenHost;

// ============================================================================
// GameScreen — 屏幕上的一种独立交互状态
// ============================================================================
//
// 屏幕之间通过 IScreenHost 在栈上管理：
//
//   switch_to(type)  — 清空栈，创建新屏幕替换整个栈（用于菜单 ↔ 游戏等全屏切换）
//   push_screen(type) — 将新屏幕压入栈顶，下层屏幕收到 suspend()（用于游戏内暂停覆盖层）
//   pop_screen()     — 弹出栈顶，下层屏幕收到 resume() 恢复状态
//
//   注意：
//   - 永远只有栈顶屏幕的 update() 和 render() 被调用
//   - 游戏屏幕被 push 覆盖时状态保留在内存中，pop 后完整恢复
//   - 不要从 GameScreen 析构函数中请求栈操作（此时 ScreenRunner 已把当前屏幕从栈中移除）
//   - 栈至少包含一个屏幕，pop_screen() 在栈深度为 1 时是 no-op
//
// 生命周期（在 ScreenRunner::tick() 中驱动）：
//   创建  → enter() → update()/render() → ... → exit() → 销毁
//                          ↓
//                    push_screen 后 suspend()
//                          ↓
//                    暂停中（不接收 update/render）
//                          ↓
//                    pop_screen 后 resume()
//                          ↓
//                    update()/render() 继续
// ============================================================================

class GameScreen {
public:
	GameScreen() = default;
	GameScreen(const GameScreen&) = delete;
	GameScreen(GameScreen&&) = delete;
	GameScreen& operator=(const GameScreen&) = delete;
	GameScreen& operator=(GameScreen&&) = delete;
	virtual ~GameScreen() = default;

	// 首次进入屏幕（或 switch_to 替换为当前屏幕时）调用
	virtual void enter(IPlatform& platform, IScreenHost& host) {}

	// 从栈中移除时调用（栈弹出 / switch_to 替换整个栈）
	virtual void exit(IPlatform& platform, IScreenHost& host) {}

	// 被新屏幕 push 覆盖时调用（栈中下层屏幕被隐藏但不销毁）
	virtual void suspend(IPlatform& platform, IScreenHost& host) {}

	// 上层屏幕 pop 后恢复时调用（重新成为栈顶）
	virtual void resume(IPlatform& platform, IScreenHost& host) {}

	// 每帧更新屏幕状态（只调用栈顶）
	virtual void update(IPlatform& platform, IScreenHost& host) = 0;

	// 渲染当前帧（只调用栈顶）
	virtual void render(IPlatform& platform, IScreenHost& host) = 0;
};

} // namespace handheld

#endif
