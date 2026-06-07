#ifndef CORE_MATH_PRNG_H
#define CORE_MATH_PRNG_H

#include <cstdint>

namespace handheld {

// xorshift32 — Marsaglia 2003。5 screens 共用，消除 ~25B/重复。
inline uint32_t xorshift32(uint32_t& state) {
    state ^= state << 13U;
    state ^= state >> 17U;
    state ^= state << 5U;
    return state;
}

} // namespace handheld

#endif
