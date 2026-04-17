#ifndef CORE_COMMON_BUTTON_STATE_H
#define CORE_COMMON_BUTTON_STATE_H

#include "core/common/Button.h"

namespace handheld {

struct ButtonState {
	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;
	bool a = false;
	bool b = false;
	bool start = false;
	bool select = false;

	[[nodiscard]] constexpr bool test(Button button) const {
		switch (button) {
		case Button::UP:
			return up;
		case Button::DOWN:
			return down;
		case Button::LEFT:
			return left;
		case Button::RIGHT:
			return right;
		case Button::A:
			return a;
		case Button::B:
			return b;
		case Button::START:
			return start;
		case Button::SELECT:
			return select;
		}

		return false;
	}

	constexpr void set(Button button, bool pressed) {
		switch (button) {
		case Button::UP:
			up = pressed;
			return;
		case Button::DOWN:
			down = pressed;
			return;
		case Button::LEFT:
			left = pressed;
			return;
		case Button::RIGHT:
			right = pressed;
			return;
		case Button::A:
			a = pressed;
			return;
		case Button::B:
			b = pressed;
			return;
		case Button::START:
			start = pressed;
			return;
		case Button::SELECT:
			select = pressed;
			return;
		}
	}

	[[nodiscard]] constexpr bool any() const {
		return up || down || left || right || a || b || start || select;
	}
};

constexpr bool operator==(const ButtonState& lhs, const ButtonState& rhs) {
	return lhs.up == rhs.up &&
		lhs.down == rhs.down &&
		lhs.left == rhs.left &&
		lhs.right == rhs.right &&
		lhs.a == rhs.a &&
		lhs.b == rhs.b &&
		lhs.start == rhs.start &&
		lhs.select == rhs.select;
}

constexpr bool operator!=(const ButtonState& lhs, const ButtonState& rhs) {
	return !(lhs == rhs);
};

} // namespace handheld

#endif
