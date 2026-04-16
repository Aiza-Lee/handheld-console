#ifndef PLATFORM_INTERFACES_IDISPLAY_H
#define PLATFORM_INTERFACES_IDISPLAY_H

#include "core/graphics/Color.h"
#include "core/graphics/Geometry.h"

#include <stdint.h>

namespace handheld {

// 显示接口
class IDisplay {
public:
	virtual ~IDisplay() = default;

	// 返回显示宽度
	virtual int16_t width() const = 0;

	// 返回显示高度
	virtual int16_t height() const = 0;

	// 清空当前帧
	virtual void clear(Color color) = 0;

	// 绘制单个像素
	virtual void draw_pixel(int16_t x, int16_t y, Color color) = 0;

	// 提交当前帧
	virtual void present() = 0;

	Size size() const {
		return {width(), height()};
	}

	void draw_pixel(Point point, Color color) {
		draw_pixel(point.x, point.y, color);
	}

	void draw_h_line(int16_t x, int16_t y, int16_t length, Color color) {
		if (length <= 0) {
			return;
		}

		for (int16_t offset = 0; offset < length; ++offset) {
			draw_pixel(static_cast<int16_t>(x + offset), y, color);
		}
	}

	void draw_v_line(int16_t x, int16_t y, int16_t length, Color color) {
		if (length <= 0) {
			return;
		}

		for (int16_t offset = 0; offset < length; ++offset) {
			draw_pixel(x, static_cast<int16_t>(y + offset), color);
		}
	}

	void draw_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, Color color) {
		int32_t currentX = x0;
		int32_t currentY = y0;
		const int32_t targetX = x1;
		const int32_t targetY = y1;
		const int32_t deltaX = (targetX >= currentX) ? (targetX - currentX) : (currentX - targetX);
		const int32_t deltaY = (targetY >= currentY) ? (targetY - currentY) : (currentY - targetY);
		const int32_t stepX = (currentX < targetX) ? 1 : -1;
		const int32_t stepY = (currentY < targetY) ? 1 : -1;
		int32_t error = deltaX - deltaY;

		while (true) {
			draw_pixel(static_cast<int16_t>(currentX), static_cast<int16_t>(currentY), color);
			if (currentX == targetX && currentY == targetY) {
				return;
			}

			const int32_t doubledError = error * 2;
			if (doubledError > -deltaY) {
				error -= deltaY;
				currentX += stepX;
			}
			if (doubledError < deltaX) {
				error += deltaX;
				currentY += stepY;
			}
		}
	}

	void draw_rect(const Rect& rect, Color color) {
		if (rect.empty()) {
			return;
		}

		draw_h_line(rect.x, rect.y, rect.width, color);
		draw_h_line(rect.x, static_cast<int16_t>(rect.y + rect.height - 1), rect.width, color);
		draw_v_line(rect.x, rect.y, rect.height, color);
		draw_v_line(static_cast<int16_t>(rect.x + rect.width - 1), rect.y, rect.height, color);
	}

	void fill_rect(const Rect& rect, Color color) {
		if (rect.empty()) {
			return;
		}

		for (int16_t row = 0; row < rect.height; ++row) {
			draw_h_line(rect.x, static_cast<int16_t>(rect.y + row), rect.width, color);
		}
	}

	void draw_bitmap(Point origin, const Color* pixels, int16_t bitmap_width, int16_t bitmap_height,
			uint16_t stride_pixels = 0) {
		if (pixels == nullptr || bitmap_width <= 0 || bitmap_height <= 0) {
			return;
		}

		const uint16_t stride = (stride_pixels == 0) ? static_cast<uint16_t>(bitmap_width) : stride_pixels;
		for (int16_t row = 0; row < bitmap_height; ++row) {
			for (int16_t column = 0; column < bitmap_width; ++column) {
				const uint16_t index =
					static_cast<uint16_t>(row) * stride + static_cast<uint16_t>(column);
				draw_pixel(static_cast<int16_t>(origin.x + column),
					static_cast<int16_t>(origin.y + row),
					pixels[index]);
			}
		}
	}
};

} // namespace handheld

#endif
