#ifndef CORE_RUNTIME_SCREEN_RUNNER_H
#define CORE_RUNTIME_SCREEN_RUNNER_H

#include "core/runtime/IScreenHost.h"
#include "core/runtime/GameScreen.h"
#include "core/runtime/ScreenType.h"
#include <memory>
#include <vector>

namespace handheld {

class IScreenFactory;

// 驱动屏幕生命周期、输入采样和单帧提交。
// 屏幕以栈组织：只有栈顶屏幕接收 update/render，
// 下层屏幕保持挂起状态（suspend/resume）。
class ScreenRunner : public IScreenHost {
public:
	ScreenRunner(IPlatform& platform, IScreenFactory& factory, ScreenType initial_screen,
	             uint32_t frame_time_ms = 67);
	ScreenRunner(const ScreenRunner&) = delete;
	ScreenRunner(ScreenRunner&&) = delete;
	ScreenRunner& operator=(const ScreenRunner&) = delete;
	ScreenRunner& operator=(ScreenRunner&&) = delete;
	~ScreenRunner() = default;

	void tick();
	void run_forever();
	void switch_to(ScreenType type) override;
	void push_screen(ScreenType type) override;
	void pop_screen() override;

private:
	enum class PendingOp { NONE, SWITCH, PUSH, POP };

	void _apply_pending();

	IPlatform& _platform;
	IScreenFactory& _factory;
	std::vector<std::unique_ptr<GameScreen>> _stack;
	ScreenType _pending_type;
	PendingOp _pending_op = PendingOp::NONE;
	uint32_t _frame_time_ms;
	uint32_t _last_frame_tick = 0;
	bool _entered = false;
};

} // namespace handheld

#endif
