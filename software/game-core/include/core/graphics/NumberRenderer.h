#ifndef CORE_GRAPHICS_NUMBER_RENDERER_H
#define CORE_GRAPHICS_NUMBER_RENDERER_H

#include <cstdint>

namespace handheld {

// 轻量级整数转字符串，替代 snprintf（在 MCU 上可节省 ~1-3KB 代码）
inline const char* itoa_dec(uint16_t value, char* buf, std::size_t buf_size) {
    if (buf_size < 2) return buf;
    char* p = buf;
    if (value == 0) {
        *p++ = '0';
        *p = '\0';
        return buf;
    }
    // 从后往前填充
    char tmp[6];
    uint8_t len = 0;
    while (value > 0 && len < 5) {
        tmp[len++] = static_cast<char>('0' + (value % 10));
        value /= 10;
    }
    for (uint8_t i = len; i > 0 && static_cast<std::size_t>(p - buf) < buf_size - 1; --i) {
        *p++ = tmp[i - 1];
    }
    *p = '\0';
    return buf;
}

} // namespace handheld

#endif
