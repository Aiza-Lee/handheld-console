#ifndef CORE_AUDIO_SOUNDS_H
#define CORE_AUDIO_SOUNDS_H

#include <cstddef>
#include <cstdint>

namespace handheld {

struct Tone {
    uint16_t frequencyHz = 0;
    uint16_t durationMs = 0;
};

namespace sounds {

// ── BGM ─────────────────────────────────────────────────────────

inline constexpr Tone BGM_MENU[] = {
    {261, 400}, {329, 300}, {391, 400}, {440, 300}, {391, 500}, {0, 400},
    {329, 300}, {391, 300}, {440, 400}, {523, 300}, {440, 500}, {0, 400},
    {293, 400}, {349, 300}, {440, 400}, {391, 300}, {349, 400}, {329, 500}, {0, 400},
    {523, 400}, {440, 300}, {391, 400}, {329, 300}, {293, 400}, {261, 600}, {0, 600},
};
inline constexpr size_t BGM_MENU_COUNT = 26;

inline constexpr Tone BGM_GUIDE[] = {
    {261, 500}, {391, 400}, {329, 500}, {523, 400}, {391, 500}, {0, 350},
    {349, 400}, {440, 300}, {523, 400}, {440, 300}, {349, 500}, {0, 350},
    {329, 400}, {391, 300}, {523, 400}, {391, 300}, {329, 500}, {261, 700}, {0, 700},
};
inline constexpr size_t BGM_GUIDE_COUNT = 19;

inline constexpr Tone BGM_PLAYGROUND[] = {
    {261, 350}, {293, 250}, {391, 400}, {329, 250}, {523, 400}, {391, 350},
    {329, 500}, {0, 350}, {220, 300}, {261, 250}, {329, 350}, {391, 300},
    {293, 250}, {261, 350}, {195, 600}, {0, 400}, {349, 350}, {440, 250},
    {523, 400}, {391, 300}, {329, 350}, {293, 250}, {261, 500}, {0, 400},
    {391, 300}, {329, 250}, {523, 350}, {440, 300}, {391, 250}, {329, 350},
    {261, 600}, {0, 600},
};
inline constexpr size_t BGM_PLAYGROUND_COUNT = 32;

inline constexpr Tone BGM_SNAKE[] = {
    {329, 250}, {391, 200}, {440, 300}, {391, 200}, {523, 350}, {440, 250},
    {391, 300}, {0, 300}, {329, 250}, {391, 200}, {440, 300}, {523, 250},
    {587, 300}, {523, 250}, {440, 350}, {0, 300}, {164, 350}, {195, 250},
    {261, 400}, {293, 250}, {329, 350}, {391, 300}, {523, 400}, {0, 350},
    {391, 250}, {329, 200}, {523, 300}, {440, 250}, {391, 300}, {329, 350},
    {523, 500}, {0, 500},
};
inline constexpr size_t BGM_SNAKE_COUNT = 32;

inline constexpr Tone BGM_PACMAN[] = {
    {523, 300}, {659, 250}, {783, 350}, {1046, 300}, {783, 250}, {659, 300},
    {523, 400}, {0, 300}, {440, 300}, {523, 250}, {659, 350}, {880, 300},
    {659, 250}, {523, 300}, {440, 400}, {0, 300}, {349, 350}, {440, 250},
    {523, 400}, {698, 300}, {659, 250}, {523, 300}, {391, 400}, {0, 350},
    {329, 300}, {391, 250}, {523, 350}, {659, 300}, {523, 400}, {391, 500},
    {261, 700}, {0, 600},
};
inline constexpr size_t BGM_PACMAN_COUNT = 32;

inline constexpr Tone BGM_BREAKOUT[] = {
    {391, 300}, {523, 250}, {391, 300}, {587, 350}, {523, 300}, {391, 250},
    {659, 400}, {0, 300}, {587, 300}, {523, 250}, {440, 350}, {391, 250},
    {329, 350}, {293, 250}, {261, 400}, {0, 350}, {329, 350}, {391, 250},
    {523, 400}, {659, 300}, {587, 250}, {523, 350}, {391, 450}, {0, 350},
    {293, 400}, {391, 300}, {523, 400}, {391, 350}, {329, 400}, {261, 600}, {0, 600},
};
inline constexpr size_t BGM_BREAKOUT_COUNT = 31;

inline constexpr Tone BGM_INVADERS[] = {
    {391, 300}, {349, 300}, {311, 300}, {293, 500}, {0, 400},
    {391, 300}, {349, 300}, {311, 300}, {293, 500}, {0, 400},
    {391, 250}, {349, 250}, {311, 250}, {293, 400}, {0, 300},
    {391, 250}, {349, 250}, {311, 250}, {293, 400}, {0, 300},
    {523, 250}, {391, 250}, {349, 300}, {311, 300}, {293, 450}, {0, 400},
    {391, 350}, {349, 300}, {293, 500}, {0, 500}, {195, 600}, {0, 700},
};
inline constexpr size_t BGM_INVADERS_COUNT = 32;

inline constexpr Tone BGM_GROWBALL[] = {
    {261, 150}, {329, 150}, {391, 200}, {523, 300}, {0, 100},
    {329, 150}, {391, 150}, {523, 200}, {659, 400}, {0, 200},
    {391, 150}, {329, 150}, {261, 200}, {195, 400}, {0, 300},
};
inline constexpr size_t BGM_GROWBALL_COUNT = 15;

// ── SFX ─────────────────────────────────────────────────────────

inline constexpr Tone BOOT[] = {
    {0, 500}, {261, 100}, {329, 100}, {391, 100}, {523, 150},
    {0, 100}, {523, 80}, {391, 80}, {329, 80}, {261, 300},
};
inline constexpr size_t BOOT_COUNT = 10;

inline constexpr Tone SFX_SELECT[] = {
    {523, 30},
};
inline constexpr size_t SFX_SELECT_COUNT = 1;

inline constexpr Tone SFX_CONFIRM[] = {
    {523, 50}, {659, 50}, {783, 80},
};
inline constexpr size_t SFX_CONFIRM_COUNT = 3;

inline constexpr Tone SFX_EAT[] = {
    {523, 30}, {659, 30},
};
inline constexpr size_t SFX_EAT_COUNT = 2;

inline constexpr Tone SFX_DEATH_SNAKE[] = {
    {391, 100}, {329, 100}, {261, 150}, {195, 200},
};
inline constexpr size_t SFX_DEATH_SNAKE_COUNT = 4;

inline constexpr Tone SFX_CHOMP[] = {
    {659, 40}, {0, 15}, {523, 40}, {0, 15}, {659, 40}, {0, 15}, {523, 40}, {0, 15},
};
inline constexpr size_t SFX_CHOMP_COUNT = 8;

inline constexpr Tone SFX_DEATH_PACMAN[] = {
    {987, 100}, {880, 80}, {783, 80}, {698, 80}, {659, 100},
    {587, 100}, {523, 150}, {391, 200}, {261, 300},
};
inline constexpr size_t SFX_DEATH_PACMAN_COUNT = 9;

inline constexpr Tone SFX_POWER_PELLET[] = {
    {523, 40}, {659, 40}, {783, 40}, {1046, 60}, {1318, 60}, {1567, 80}, {2093, 100},
};
inline constexpr size_t SFX_POWER_PELLET_COUNT = 7;

inline constexpr Tone SFX_BRICK[] = {
    {1046, 25}, {1318, 15},
};
inline constexpr size_t SFX_BRICK_COUNT = 2;

inline constexpr Tone SFX_PADDLE[] = {
    {783, 25}, {1046, 15},
};
inline constexpr size_t SFX_PADDLE_COUNT = 2;

inline constexpr Tone SFX_DEATH_BREAKOUT[] = {
    {659, 100}, {523, 100}, {391, 150}, {329, 200},
};
inline constexpr size_t SFX_DEATH_BREAKOUT_COUNT = 4;

inline constexpr Tone SFX_SHOOT[] = {
    {1046, 30},
};
inline constexpr size_t SFX_SHOOT_COUNT = 1;

inline constexpr Tone SFX_EXPLOSION[] = {
    {261, 50}, {195, 40}, {130, 60}, {0, 20}, {200, 40},
};
inline constexpr size_t SFX_EXPLOSION_COUNT = 5;

inline constexpr Tone SFX_EAT_GROWBALL[] = {
    {523, 30}, {659, 40}, {783, 30},
};
inline constexpr size_t SFX_EAT_GROWBALL_COUNT = 3;

inline constexpr Tone SFX_EAT_ENEMY_GROWBALL[] = {
    {261, 40}, {329, 40}, {391, 50}, {523, 60}, {659, 80}, {783, 100}, {0, 30},
};
inline constexpr size_t SFX_EAT_ENEMY_GROWBALL_COUNT = 7;

inline constexpr Tone SFX_DEATH_GROWBALL[] = {
    {391, 100}, {329, 100}, {261, 150}, {195, 200}, {0, 100}, {130, 300},
};
inline constexpr size_t SFX_DEATH_GROWBALL_COUNT = 6;

inline constexpr Tone SFX_VICTORY_GROWBALL[] = {
    {523, 80}, {659, 80}, {783, 80}, {1046, 150}, {0, 50}, {1046, 80}, {1318, 200},
};
inline constexpr size_t SFX_VICTORY_GROWBALL_COUNT = 7;

} // namespace sounds
} // namespace handheld

#endif
