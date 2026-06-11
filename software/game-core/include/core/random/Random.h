#ifndef HANDHELD_CORE_RANDOM_H
#define HANDHELD_CORE_RANDOM_H

#include <cstdint>

namespace handheld::random {

inline uint32_t g_state = 0xC0FFEEu;

inline void seed(uint32_t s) {
    g_state = (s != 0u) ? s : 1u;
}

inline uint32_t next() {
    uint32_t x = g_state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    g_state = x;
    return x;
}

inline uint32_t next_in_range(uint32_t max) {
    return next() % max;
}

} // namespace handheld::random

#endif // HANDHELD_CORE_RANDOM_H
