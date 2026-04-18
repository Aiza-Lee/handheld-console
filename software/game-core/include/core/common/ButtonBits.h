#ifndef CORE_COMMON_BUTTON_H
#define CORE_COMMON_BUTTON_H

#include <cstdint>

namespace handheld {

enum class ButtonBits : uint8_t {
	NONE   = 0,
	UP     = 1U << 0,
	DOWN   = 1U << 1,
	LEFT   = 1U << 2,
	RIGHT  = 1U << 3,
	A      = 1U << 4,
	B      = 1U << 5,
	START  = 1U << 6,
	SELECT = 1U << 7,
};

[[nodiscard]] constexpr uint8_t button_bits(ButtonBits button) {
	return static_cast<uint8_t>(button);
}

[[nodiscard]] constexpr ButtonBits operator|(ButtonBits lhs, ButtonBits rhs) {
	return static_cast<ButtonBits>(button_bits(lhs) | button_bits(rhs));
}

[[nodiscard]] constexpr ButtonBits operator&(ButtonBits lhs, ButtonBits rhs) {
	return static_cast<ButtonBits>(button_bits(lhs) & button_bits(rhs));
}

constexpr ButtonBits& operator|=(ButtonBits& lhs, ButtonBits rhs) {
	lhs = lhs | rhs;
	return lhs;
}

constexpr ButtonBits& operator&=(ButtonBits& lhs, ButtonBits rhs) {
	lhs = lhs & rhs;
	return lhs;
}

} // namespace handheld

#endif
