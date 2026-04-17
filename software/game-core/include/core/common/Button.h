#ifndef CORE_COMMON_BUTTON_H
#define CORE_COMMON_BUTTON_H

#include <cstdint>

namespace handheld {

enum class Button : uint8_t {
	UP = 0,
	DOWN,
	LEFT,
	RIGHT,
	A,
	B,
	START,
	SELECT,
};

} // namespace handheld

#endif
