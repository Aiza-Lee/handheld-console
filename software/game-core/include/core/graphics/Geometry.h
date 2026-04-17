#ifndef CORE_GRAPHICS_GEOMETRY_H
#define CORE_GRAPHICS_GEOMETRY_H

#include <cstdint>

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

	[[nodiscard]] constexpr bool empty() const {
		return width <= 0 || height <= 0;
	}

	[[nodiscard]] constexpr int16_t left() const { return x; }
	[[nodiscard]] constexpr int16_t top() const { return y; }
	[[nodiscard]] constexpr int16_t right() const { return static_cast<int16_t>(x + width); }
	[[nodiscard]] constexpr int16_t bottom() const { return static_cast<int16_t>(y + height); }
};

} // namespace handheld

#endif
