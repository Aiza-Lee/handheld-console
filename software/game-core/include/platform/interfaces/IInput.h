#ifndef PLATFORM_INTERFACES_IINPUT_H
#define PLATFORM_INTERFACES_IINPUT_H

#include "core/common/ButtonBits.h"
#include "core/common/ButtonState.h"

namespace handheld {

// 输入接口
class IInput {
public:
	IInput() = default;
	IInput(const IInput&) = delete;
	IInput(IInput&&) = delete;
	IInput& operator=(const IInput&) = delete;
	IInput& operator=(IInput&&) = delete;
	virtual ~IInput() = default;

	// 刷新输入状态
	virtual void poll() = 0;

	// 返回当前帧按键状态
	[[nodiscard]] virtual ButtonState current_buttons() const = 0;

	// 返回上一帧按键状态
	[[nodiscard]] virtual ButtonState previous_buttons() const = 0;

	[[nodiscard]] ButtonState read_buttons() const {
		return current_buttons();
	}

	[[nodiscard]] bool is_down(ButtonBits button) const {
		return current_buttons().test(button);
	}

	[[nodiscard]] bool was_pressed(ButtonBits button) const {
		const ButtonState current = current_buttons();
		const ButtonState previous = previous_buttons();
		return current.test(button) && !previous.test(button);
	}

	[[nodiscard]] bool was_released(ButtonBits button) const {
		const ButtonState current = current_buttons();
		const ButtonState previous = previous_buttons();
		return !current.test(button) && previous.test(button);
	}

	[[nodiscard]] bool changed() const {
		return current_buttons() != previous_buttons();
	}
};

} // namespace handheld

#endif
