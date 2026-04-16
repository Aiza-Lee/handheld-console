#ifndef CORE_GRAPHICS_GEOMETRY_H
#define CORE_GRAPHICS_GEOMETRY_H

#include <stdint.h>

namespace handheld {

struct Point {
	int16_t x = 0;
	int16_t y = 0;
};

struct Size {
	int16_t width = 0;
	int16_t height = 0;
};

struct Rect {
	int16_t x = 0;
	int16_t y = 0;
	int16_t width = 0;
	int16_t height = 0;

	constexpr bool empty() const {
		return width <= 0 || height <= 0;
	}

	constexpr int16_t left() const { return x; }
	constexpr int16_t top() const { return y; }
	constexpr int16_t right() const { return static_cast<int16_t>(x + width); }
	constexpr int16_t bottom() const { return static_cast<int16_t>(y + height); }
};

} // namespace handheld

#endif
