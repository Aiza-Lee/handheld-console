#ifndef CORE_GRAPHICS_NUMBER_RENDERER_H
#define CORE_GRAPHICS_NUMBER_RENDERER_H

#include <cstdint>

namespace handheld {

// 轻量级整数转字符串，替代 snprintf（在 MCU 上可节省 ~1-3KB 代码）
// 返回写入位置之后的指针（指向 '\0'）。
inline char* itoa_dec(int32_t value, char* buf) {
    char* p = buf;
    if (value < 0) {
        *p++ = '-';
        auto u = static_cast<uint32_t>(-(value + 1)) + 1U;
        char tmp[10]; uint8_t len = 0;
        do { tmp[len++] = static_cast<char>('0' + (u % 10)); u /= 10; } while (u > 0);
        for (uint8_t i = len; i > 0; --i) *p++ = tmp[i - 1];
    } else {
        auto u = static_cast<uint32_t>(value);
        char tmp[10]; uint8_t len = 0;
        do { tmp[len++] = static_cast<char>('0' + (u % 10)); u /= 10; } while (u > 0);
        for (uint8_t i = len; i > 0; --i) *p++ = tmp[i - 1];
    }
    *p = '\0';
    return p;
}

inline char* itoa_dec(uint32_t value, char* buf) {
    char tmp[10]; uint8_t len = 0;
    do { tmp[len++] = static_cast<char>('0' + (value % 10)); value /= 10; } while (value > 0);
    char* p = buf;
    for (uint8_t i = len; i > 0; --i) *p++ = tmp[i - 1];
    *p = '\0';
    return p;
}

inline char* itoa_dec(uint16_t value, char* buf) {
    return itoa_dec(static_cast<uint32_t>(value), buf);
}

// 小写十六进制；width=0 按实际位数；zero_pad=true 时左侧补 '0' 至 width。
inline char* itoa_hex(uint32_t value, char* buf, uint8_t width = 0, bool zero_pad = false) {
    char tmp[8]; uint8_t len = 0;
    do {
        uint8_t d = value & 0xFU;
        tmp[len++] = static_cast<char>(d < 10 ? '0' + d : 'a' + (d - 10));
        value >>= 4;
    } while (value > 0);
    while (len < width) tmp[len++] = '0';
    char* p = buf;
    for (uint8_t i = len; i > 0; --i) *p++ = tmp[i - 1];
    *p = '\0';
    return p;
}

} // namespace handheld

#endif
