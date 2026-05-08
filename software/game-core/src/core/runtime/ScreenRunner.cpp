#include "core/runtime/ScreenRunner.h"
#include "core/runtime/ScreenFactory.h"

namespace handheld {

ScreenRunner::ScreenRunner(IPlatform& platform, IScreenFactory& factory, ScreenType initial_screen,
                           uint32_t frame_time_ms)
	: _platform(platform),
	  _factory(factory),
	  _pending_type(initial_screen),
	  _pending_op(PendingOp::SWITCH),
	  _frame_time_ms(frame_time_ms) {}

// ---- IScreenHost 实现 ----

void ScreenRunner::switch_to(ScreenType type) {
	_pending_type = type;
	_pending_op = PendingOp::SWITCH;
}

void ScreenRunner::push_screen(ScreenType type) {
	_pending_type = type;
	_pending_op = PendingOp::PUSH;
}

void ScreenRunner::pop_screen() {
	_pending_op = PendingOp::POP;
}

// ---- 内部调度 ----

void ScreenRunner::_apply_pending() {
	switch (_pending_op) {
	case PendingOp::SWITCH: {
		// 退出并销毁栈中所有屏幕
		if (!_stack.empty()) {
			if (_entered) {
				_stack.back()->exit(_platform, *this);
			}
			_stack.clear();
		}
		_stack.push_back(_factory.create(_pending_type));
		_pending_op = PendingOp::NONE;
		_entered = false;
		break;
	}
	case PendingOp::PUSH: {
		// 挂起当前栈顶，压入新屏幕
		if (!_stack.empty() && _entered) {
			_stack.back()->suspend(_platform, *this);
		}
		_stack.push_back(_factory.create(_pending_type));
		_pending_op = PendingOp::NONE;
		_entered = false;
		break;
	}
	case PendingOp::POP: {
		// 弹出栈顶，恢复下层屏幕
		if (_stack.size() <= 1) {
			_pending_op = PendingOp::NONE;
			break;
		}
		if (_entered) {
			_stack.back()->exit(_platform, *this);
		}
		_stack.pop_back();
		_pending_op = PendingOp::NONE;
		// 恢复刚回到栈顶的屏幕
		_stack.back()->resume(_platform, *this);
		// _entered 保持 true：resume 相当于重新 enter
		break;
	}
	case PendingOp::NONE:
		break;
	}
}

void ScreenRunner::tick() {
	_apply_pending();

	GameScreen& top = *_stack.back();
	if (!_entered) {
		top.enter(_platform, *this);
		_entered = true;
	}

	_platform.input().poll();
	top.update(_platform, *this);
	top.render(_platform, *this);

	_platform.display().present();

	_apply_pending();
}

void ScreenRunner::run_forever() {
	_last_frame_tick = _platform.time().ticks_ms();
	for (;;) {
		tick();

		if (_frame_time_ms > 0) {
			const uint32_t now = _platform.time().ticks_ms();
			const uint32_t elapsed = now - _last_frame_tick;
			if (elapsed < _frame_time_ms) {
				_platform.time().delay_ms(_frame_time_ms - elapsed);
			}
			_last_frame_tick = _platform.time().ticks_ms();
		}
	}
}

}  // namespace handheld
