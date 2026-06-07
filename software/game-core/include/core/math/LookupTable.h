#ifndef CORE_MATH_LOOKUP_TABLE_H
#define CORE_MATH_LOOKUP_TABLE_H

#include <array>
#include <cstdint>

namespace handheld {

// constexpr sin via Taylor series, accurate to ~1e-6 on [-pi, pi]
constexpr double constexpr_sin(double x) {
    constexpr double PI = 3.14159265358979323846;
    constexpr double TWO_PI = 2.0 * PI;
    if (x > PI) {
        x -= TWO_PI;
    }
    double x2 = x * x;
    double term = x;
    double result = term;
    term *= -x2 / 6.0; // -x^3/3!
    result += term;
    term *= -x2 / 20.0; // x^5/5!
    result += term;
    term *= -x2 / 42.0; // -x^7/7!
    result += term;
    term *= -x2 / 72.0; // x^9/9!
    result += term;
    term *= -x2 / 110.0; // -x^11/11!
    result += term;
    return result;
}

// 生成 256 项正弦查找表，范围 [-127, 127]
constexpr std::array<int8_t, 256> make_sin_lut() {
    std::array<int8_t, 256> table{};
    constexpr double SCALE = 127.0;
    constexpr double STEP = 2.0 * 3.14159265358979323846 / 256.0;
    for (int i = 0; i < 256; ++i) {
        double v = constexpr_sin(static_cast<double>(i) * STEP) * SCALE;
        table[i] = static_cast<int8_t>(static_cast<int>(v + (v >= 0.0 ? 0.5 : -0.5)));
    }
    return table;
}

inline constexpr std::array<int8_t, 256> SIN_LUT = make_sin_lut();

// sin(2*pi*phase/256) * scale / 128  (phase in [0,255], scale = amplitude)
inline int16_t sin_lut(uint8_t phase, int16_t scale) { return static_cast<int32_t>(SIN_LUT[phase]) * scale / 128; }

// cos(2*pi*phase/256) * scale / 128  (cos = sin shifted by 64 = 90 degrees)
inline int16_t cos_lut(uint8_t phase, int16_t scale) { return sin_lut(static_cast<uint8_t>(phase + 64U), scale); }

// 归一化到 [-1.0, 1.0]，用于浮点场景
inline double sin_lut_double(uint8_t phase) { return static_cast<double>(SIN_LUT[phase]) / 128.0; }

inline double cos_lut_double(uint8_t phase) { return sin_lut_double(static_cast<uint8_t>(phase + 64U)); }

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// 将弧度值转为 256-phase 索引
inline uint8_t phase_from_radians(double radians) {
    return static_cast<uint8_t>(static_cast<int64_t>(radians * (128.0 / M_PI)) & 0xFF);
}

} // namespace handheld

#endif
