#ifndef PLATFORM_INTERFACES_IDISPLAY_H
#define PLATFORM_INTERFACES_IDISPLAY_H

#include "core/graphics/Color.h"
#include "core/graphics/Geometry.h"

#include <cstdint>

namespace handheld {

// 显示接口
class IDisplay {
public:
    IDisplay() = default;
    IDisplay(const IDisplay&) = delete;
    IDisplay(IDisplay&&) = delete;
    IDisplay& operator=(const IDisplay&) = delete;
    IDisplay& operator=(IDisplay&&) = delete;
    virtual ~IDisplay() = default;

    // 返回显示宽度
    [[nodiscard]] virtual int16_t width() const = 0;

    // 返回显示高度
    [[nodiscard]] virtual int16_t height() const = 0;

    // 清空当前帧
    virtual void clear(Color color) = 0;

    // 绘制单个像素
    virtual void draw_pixel(int16_t x, int16_t y, Color color) = 0;

    void fill_rect(const Rect& rect, Color color) {
        if (rect.empty()) return;
        for (int16_t row = 0; row < rect.height; ++row) {
            draw_h_line(rect.x, static_cast<int16_t>(rect.y + row), rect.width, color);
        }
    }

    // 提交当前帧
    virtual void present() = 0;

    [[nodiscard]] Size size() const { return {width(), height()}; }

    void draw_pixel(Point point, Color color) { draw_pixel(point.x, point.y, color); }

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

    void draw_rect(const Rect& rect, Color color) {
        if (rect.empty()) {
            return;
        }

        draw_h_line(rect.x, rect.y, rect.width, color);
        draw_h_line(rect.x, static_cast<int16_t>(rect.y + rect.height - 1), rect.width, color);
        draw_v_line(rect.x, rect.y, rect.height, color);
        draw_v_line(static_cast<int16_t>(rect.x + rect.width - 1), rect.y, rect.height, color);
    }

    void draw_bitmap(Point origin, const Color* pixels, int16_t bitmap_width, int16_t bitmap_height,
                     uint16_t stride_pixels = 0) {
        if (pixels == nullptr || bitmap_width <= 0 || bitmap_height <= 0) {
            return;
        }

        const uint16_t stride = (stride_pixels == 0) ? static_cast<uint16_t>(bitmap_width) : stride_pixels;
        for (int16_t row = 0; row < bitmap_height; ++row) {
            for (int16_t column = 0; column < bitmap_width; ++column) {
                const uint16_t index = (static_cast<uint16_t>(row) * stride) + static_cast<uint16_t>(column);
                draw_pixel(static_cast<int16_t>(origin.x + column), static_cast<int16_t>(origin.y + row),
                           pixels[index]);
            }
        }
    }
};

} // namespace handheld

#endif
