#ifndef CORE_COMMON_BUTTON_STATE_H
#define CORE_COMMON_BUTTON_STATE_H

#include "core/common/ButtonBits.h"

#include <cstdint>

namespace handheld {

struct ButtonState {
	constexpr ButtonState() = default;
	constexpr ButtonState(ButtonBits buttons) : _bits(button_bits(buttons)) {}

	[[nodiscard]] constexpr bool test(ButtonBits button) const {
		const uint8_t mask = button_bits(button);
		return mask != 0 && (_bits & mask) == mask;
	}

	constexpr ButtonState& set(ButtonBits button, bool pressed) {
		const uint8_t mask = button_bits(button);
		if (pressed) {
			_bits |= mask;
			return *this;
		}

		_bits &= static_cast<uint8_t>(~mask);
		return *this;
	}

	[[nodiscard]] constexpr bool any() const { return _bits != 0; }
	[[nodiscard]] constexpr bool none() const { return _bits == 0; }
	[[nodiscard]] constexpr uint8_t bits() const { return _bits; }

private:
	uint8_t _bits = 0;
};

constexpr bool operator==(const ButtonState& lhs, const ButtonState& rhs) {
	return lhs.bits() == rhs.bits();
}

constexpr bool operator!=(const ButtonState& lhs, const ButtonState& rhs) {
	return !(lhs == rhs);
};

} // namespace handheld

#endif
