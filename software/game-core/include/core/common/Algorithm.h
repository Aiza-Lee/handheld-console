#ifndef CORE_COMMON_ALGORITHM_H
#define CORE_COMMON_ALGORITHM_H

namespace handheld {

template <typename T>
inline constexpr T min(T a, T b) {
    return (b < a) ? b : a;
}

template <typename T>
inline constexpr T max(T a, T b) {
    return (a < b) ? b : a;
}

template <typename T>
inline constexpr T clamp(T v, T lo, T hi) {
    return (v < lo) ? lo : ((hi < v) ? hi : v);
}

} // namespace handheld

#endif
