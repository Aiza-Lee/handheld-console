#ifndef PLATFORM_INTERFACES_IINPUT_H
#define PLATFORM_INTERFACES_IINPUT_H

#include "core/common/Button.h"
#include "core/common/ButtonState.h"

namespace handheld {

// 输入接口
class IInput {
public:
	virtual ~IInput() = default;

	// 刷新输入状态
	virtual void poll() = 0;

	// 返回当前帧按键状态
	virtual ButtonState current_buttons() const = 0;

	// 返回上一帧按键状态
	virtual ButtonState previous_buttons() const = 0;

	ButtonState read_buttons() const {
		return current_buttons();
	}

	bool is_down(Button button) const {
		return current_buttons().test(button);
	}

	bool was_pressed(Button button) const {
		const ButtonState current = current_buttons();
		const ButtonState previous = previous_buttons();
		return current.test(button) && !previous.test(button);
	}

	bool was_released(Button button) const {
		const ButtonState current = current_buttons();
		const ButtonState previous = previous_buttons();
		return !current.test(button) && previous.test(button);
	}

	bool changed() const {
		return current_buttons() != previous_buttons();
	}
};

} // namespace handheld

#endif
