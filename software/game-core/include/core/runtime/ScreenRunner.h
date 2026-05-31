#ifndef CORE_RUNTIME_SCREEN_RUNNER_H
#define CORE_RUNTIME_SCREEN_RUNNER_H

#include "core/audio/AudioEngine.h"
#include "core/runtime/IScreenHost.h"
#include "core/runtime/GameScreen.h"
#include "core/runtime/ScreenType.h"
#include <array>
#include <cstdint>
#include <memory>

namespace handheld {

class IScreenFactory;

// 驱动屏幕生命周期、输入采样和单帧提交。
// 屏幕以栈组织：只有栈顶屏幕接收 update/render，
// 下层屏幕保持挂起状态（suspend/resume）。
class ScreenRunner : public IScreenHost {
public:
	ScreenRunner(IPlatform& platform, IScreenFactory& factory, ScreenType initial_screen,
	             uint32_t frame_time_ms = 33);
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
	[[nodiscard]] AudioEngine& audio() override { return _audio_engine; }
	[[nodiscard]] uint32_t frame_time_ms() const { return _frame_time_ms; }

private:
	enum class PendingOp { NONE, SWITCH, PUSH, POP };

	void _apply_pending();

	static constexpr size_t K_MAX_SCREENS = 4;
	static constexpr size_t K_MAX_AUDIO_SAMPLES = AudioEngine::SAMPLE_RATE * 100 / 1000;

	IPlatform& _platform;
	IScreenFactory& _factory;
	std::array<std::unique_ptr<GameScreen>, K_MAX_SCREENS> _stack{};
	uint8_t _stack_size = 0;
	ScreenType _pending_type;
	PendingOp _pending_op = PendingOp::NONE;
	uint32_t _frame_time_ms;
	uint32_t _last_frame_tick = 0;
	bool _entered = false;
	AudioEngine _audio_engine;
	std::array<int16_t, K_MAX_AUDIO_SAMPLES> _audio_buf{};
};

} // namespace handheld

#endif
