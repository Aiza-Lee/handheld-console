#ifndef CORE_GRAPHICS_COLOR_H
#define CORE_GRAPHICS_COLOR_H

#include <stdint.h>

namespace handheld {

enum class Color : uint16_t {
	BLACK   = 0x0000,
	WHITE   = 0xFFFF,
	RED     = 0xF800,
	GREEN   = 0x07E0,
	BLUE    = 0x001F,
	YELLOW  = 0xFFE0,
	CYAN    = 0x07FF,
	MAGENTA = 0xF81F,
	GRAY    = 0x8410,
};

constexpr Color rgb565(uint8_t red, uint8_t green, uint8_t blue) {
	return static_cast<Color>(
		((static_cast<uint16_t>(red) & 0xF8u) << 8) |
		((static_cast<uint16_t>(green) & 0xFCu) << 3) |
		(static_cast<uint16_t>(blue) >> 3));
}

} // namespace handheld

#endif
