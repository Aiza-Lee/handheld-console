#ifndef CORE_RUNTIME_ISCREEN_HOST_H
#define CORE_RUNTIME_ISCREEN_HOST_H

#include "core/audio/AudioEngine.h"
#include "core/runtime/ScreenType.h"

namespace handheld {

class GameScreen;

// GameScreen 通过此接口请求切换、压入或弹出屏幕。
// 所有操作延迟到当前帧的 tick() 末尾执行，
// 避免在 update() 执行过程中销毁屏幕。
class IScreenHost {
public:
	IScreenHost() = default;
	IScreenHost(const IScreenHost&) = delete;
	IScreenHost(IScreenHost&&) = delete;
	IScreenHost& operator=(const IScreenHost&) = delete;
	IScreenHost& operator=(IScreenHost&&) = delete;
	virtual ~IScreenHost() = default;

	// 清空整个栈，创建新屏幕（菜单 ↔ 游戏等全屏切换）
	virtual void switch_to(ScreenType type) = 0;

	// 将新屏幕压入栈顶，下层屏幕收到 suspend()
	virtual void push_screen(ScreenType type) = 0;

	// 弹出栈顶屏幕，恢复下层屏幕（下层收到 resume()）
	virtual void pop_screen() = 0;

	// 返回音频引擎，供 Screen 设置 BGM / 播放 SFX
	[[nodiscard]] virtual AudioEngine& audio() = 0;
};

} // namespace handheld

#endif
