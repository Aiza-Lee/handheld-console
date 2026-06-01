#ifndef CORE_AUDIO_SOUNDS_H
#define CORE_AUDIO_SOUNDS_H

#include <cstddef>
#include <cstdint>

namespace handheld {

struct Tone {
    uint16_t frequencyHz = 0;
    uint16_t durationMs = 0;
};

// 标准十二平均律音高（A4 = 440Hz）
// 频率取整到最近整数，可直接用于 Tone::frequencyHz
enum Note : uint16_t {
    REST = 0,

    // Octave 3
    C3 = 131,
    Cs3 = 139,
    D3 = 147,
    Ds3 = 156,
    E3 = 165,
    F3 = 175,
    Fs3 = 185,
    G3 = 196,
    Gs3 = 208,
    A3 = 220,
    As3 = 233,
    B3 = 247,

    // Octave 4
    C4 = 262,
    Cs4 = 277,
    D4 = 294,
    Ds4 = 311,
    E4 = 330,
    F4 = 349,
    Fs4 = 370,
    G4 = 392,
    Gs4 = 415,
    A4 = 440,
    As4 = 466,
    B4 = 494,

    // Octave 5
    C5 = 523,
    Cs5 = 554,
    D5 = 587,
    Ds5 = 622,
    E5 = 659,
    F5 = 698,
    Fs5 = 740,
    G5 = 784,
    Gs5 = 831,
    A5 = 880,
    As5 = 932,
    B5 = 988,

    // Octave 6
    C6 = 1047,
    Cs6 = 1109,
    D6 = 1175,
    Ds6 = 1245,
    E6 = 1319,
    F6 = 1397,
    Fs6 = 1480,
    G6 = 1568,
    Gs6 = 1661,
    A6 = 1760,
    As6 = 1865,
    B6 = 1976,

    // Octave 7
    C7 = 2093,
};

namespace sounds {

// ── System ────────────────────────────────────────────────────────

inline constexpr Tone BOOT[] = {
    {REST, 500}, {C4, 100}, {E4, 100}, {G4, 100}, {C5, 150},
    {REST, 100}, {C5, 80},  {G4, 80},  {E4, 80},  {C4, 300},
};
inline constexpr size_t BOOT_COUNT = 10;

inline constexpr Tone BGM_MENU[] = {
    {C4, 400}, {E4, 300}, {G4, 400}, {A4, 300}, {G4, 500}, {REST, 400},
    {E4, 300}, {G4, 300}, {A4, 400}, {C5, 300}, {A4, 500}, {REST, 400},
    {D4, 400}, {F4, 300}, {A4, 400}, {G4, 300}, {F4, 400}, {E4, 500},  {REST, 400},
    {C5, 400}, {A4, 300}, {G4, 400}, {E4, 300}, {D4, 400}, {C4, 600},  {REST, 600},
};
inline constexpr size_t BGM_MENU_COUNT = 26;

inline constexpr Tone BGM_GUIDE[] = {
    {C4, 500}, {G4, 400}, {E4, 500}, {C5, 400}, {G4, 500}, {REST, 350},
    {F4, 400}, {A4, 300}, {C5, 400}, {A4, 300}, {F4, 500}, {REST, 350},
    {E4, 400}, {G4, 300}, {C5, 400}, {G4, 300}, {E4, 500}, {C4, 700},  {REST, 700},
};
inline constexpr size_t BGM_GUIDE_COUNT = 19;

inline constexpr Tone SFX_SELECT[] = {
    {C5, 30},
};
inline constexpr size_t SFX_SELECT_COUNT = 1;

inline constexpr Tone SFX_CONFIRM[] = {
    {C5, 50}, {E5, 50}, {G5, 80},
};
inline constexpr size_t SFX_CONFIRM_COUNT = 3;

// ── Playground ────────────────────────────────────────────────────

inline constexpr Tone BGM_PLAYGROUND[] = {
    {C4, 350}, {D4, 250}, {G4, 400}, {E4, 250}, {C5, 400}, {G4, 350},
    {E4, 500}, {REST, 350}, {A3, 300}, {C4, 250}, {E4, 350}, {G4, 300},
    {D4, 250}, {C4, 350}, {G3, 600}, {REST, 400}, {F4, 350}, {A4, 250},
    {C5, 400}, {G4, 300}, {E4, 350}, {D4, 250}, {C4, 500}, {REST, 400},
    {G4, 300}, {E4, 250}, {C5, 350}, {A4, 300}, {G4, 250}, {E4, 350},
    {C4, 600}, {REST, 600},
};
inline constexpr size_t BGM_PLAYGROUND_COUNT = 32;

// ── Snake ─────────────────────────────────────────────────────────

inline constexpr Tone BGM_SNAKE[] = {
    {E4, 250}, {G4, 200}, {A4, 300}, {G4, 200}, {C5, 350}, {A4, 250},
    {G4, 300}, {REST, 300}, {E4, 250}, {G4, 200}, {A4, 300}, {C5, 250},
    {D5, 300}, {C5, 250}, {A4, 350}, {REST, 300}, {E3, 350}, {G3, 250},
    {C4, 400}, {D4, 250}, {E4, 350}, {G4, 300}, {C5, 400}, {REST, 350},
    {G4, 250}, {E4, 200}, {C5, 300}, {A4, 250}, {G4, 300}, {E4, 350},
    {C5, 500}, {REST, 500},
};
inline constexpr size_t BGM_SNAKE_COUNT = 32;

inline constexpr Tone SFX_EAT[] = {
    {C5, 30}, {E5, 30},
};
inline constexpr size_t SFX_EAT_COUNT = 2;

inline constexpr Tone SFX_DEATH_SNAKE[] = {
    {G4, 100}, {E4, 100}, {C4, 150}, {G3, 200},
};
inline constexpr size_t SFX_DEATH_SNAKE_COUNT = 4;

// ── Pac-Man ───────────────────────────────────────────────────────

inline constexpr Tone BGM_PACMAN[] = {
    {C5, 300}, {E5, 250}, {G5, 350}, {C6, 300}, {G5, 250}, {E5, 300},
    {C5, 400}, {REST, 300}, {A4, 300}, {C5, 250}, {E5, 350}, {A5, 300},
    {E5, 250}, {C5, 300}, {A4, 400}, {REST, 300}, {F4, 350}, {A4, 250},
    {C5, 400}, {F5, 300}, {E5, 250}, {C5, 300}, {G4, 400}, {REST, 350},
    {E4, 300}, {G4, 250}, {C5, 350}, {E5, 300}, {C5, 400}, {G4, 500},
    {C4, 700}, {REST, 600},
};
inline constexpr size_t BGM_PACMAN_COUNT = 32;

inline constexpr Tone SFX_CHOMP[] = {
    {E5, 40}, {REST, 15}, {C5, 40}, {REST, 15},
    {E5, 40}, {REST, 15}, {C5, 40}, {REST, 15},
};
inline constexpr size_t SFX_CHOMP_COUNT = 8;

inline constexpr Tone SFX_DEATH_PACMAN[] = {
    {B5, 100}, {A5, 80}, {G5, 80}, {F5, 80}, {E5, 100},
    {D5, 100}, {C5, 150}, {G4, 200}, {C4, 300},
};
inline constexpr size_t SFX_DEATH_PACMAN_COUNT = 9;

inline constexpr Tone SFX_POWER_PELLET[] = {
    {C5, 40}, {E5, 40}, {G5, 40}, {C6, 60}, {E6, 60}, {G6, 80}, {C7, 100},
};
inline constexpr size_t SFX_POWER_PELLET_COUNT = 7;

// ── Breakout ──────────────────────────────────────────────────────

inline constexpr Tone BGM_BREAKOUT[] = {
    {G4, 300}, {C5, 250}, {G4, 300},  {D5, 350}, {C5, 300}, {G4, 250},
    {E5, 400}, {REST, 300}, {D5, 300}, {C5, 250}, {A4, 350}, {G4, 250},
    {E4, 350}, {D4, 250},  {C4, 400}, {REST, 350}, {E4, 350}, {G4, 250},
    {C5, 400}, {E5, 300},  {D5, 250}, {C5, 350}, {G4, 450}, {REST, 350},
    {D4, 400}, {G4, 300},  {C5, 400}, {G4, 350}, {E4, 400}, {C4, 600},  {REST, 600},
};
inline constexpr size_t BGM_BREAKOUT_COUNT = 31;

inline constexpr Tone SFX_BRICK[] = {
    {C6, 25}, {E6, 15},
};
inline constexpr size_t SFX_BRICK_COUNT = 2;

inline constexpr Tone SFX_PADDLE[] = {
    {G5, 25}, {C6, 15},
};
inline constexpr size_t SFX_PADDLE_COUNT = 2;

inline constexpr Tone SFX_DEATH_BREAKOUT[] = {
    {E5, 100}, {C5, 100}, {G4, 150}, {E4, 200},
};
inline constexpr size_t SFX_DEATH_BREAKOUT_COUNT = 4;

// ── Invaders ──────────────────────────────────────────────────────

inline constexpr Tone BGM_INVADERS[] = {
    {G4, 300}, {F4, 300}, {Ds4, 300}, {D4, 500}, {REST, 400},
    {G4, 300}, {F4, 300}, {Ds4, 300}, {D4, 500}, {REST, 400},
    {G4, 250}, {F4, 250}, {Ds4, 250}, {D4, 400}, {REST, 300},
    {G4, 250}, {F4, 250}, {Ds4, 250}, {D4, 400}, {REST, 300},
    {C5, 250}, {G4, 250}, {F4, 300},  {Ds4, 300}, {D4, 450}, {REST, 400},
    {G4, 350}, {F4, 300}, {D4, 500},  {REST, 500}, {G3, 600}, {REST, 700},
};
inline constexpr size_t BGM_INVADERS_COUNT = 32;

inline constexpr Tone SFX_SHOOT[] = {
    {C6, 30},
};
inline constexpr size_t SFX_SHOOT_COUNT = 1;

inline constexpr Tone SFX_EXPLOSION[] = {
    {C4, 50}, {G3, 40}, {C3, 60}, {REST, 20}, {G3, 40},
};
inline constexpr size_t SFX_EXPLOSION_COUNT = 5;

// ── Growball ──────────────────────────────────────────────────────

inline constexpr Tone BGM_GROWBALL[] = {
    {C4, 150}, {E4, 150}, {G4, 200}, {C5, 300}, {REST, 100},
    {E4, 150}, {G4, 150}, {C5, 200}, {E5, 400}, {REST, 200},
    {G4, 150}, {E4, 150}, {C4, 200}, {G3, 400}, {REST, 300},
};
inline constexpr size_t BGM_GROWBALL_COUNT = 15;

inline constexpr Tone SFX_EAT_GROWBALL[] = {
    {C5, 30}, {E5, 40}, {G5, 30},
};
inline constexpr size_t SFX_EAT_GROWBALL_COUNT = 3;

inline constexpr Tone SFX_EAT_ENEMY_GROWBALL[] = {
    {C4, 40}, {E4, 40}, {G4, 50}, {C5, 60}, {E5, 80}, {G5, 100}, {REST, 30},
};
inline constexpr size_t SFX_EAT_ENEMY_GROWBALL_COUNT = 7;

inline constexpr Tone SFX_DEATH_GROWBALL[] = {
    {G4, 100}, {E4, 100}, {C4, 150}, {G3, 200}, {REST, 100}, {C3, 300},
};
inline constexpr size_t SFX_DEATH_GROWBALL_COUNT = 6;

inline constexpr Tone SFX_VICTORY_GROWBALL[] = {
    {C5, 80}, {E5, 80}, {G5, 80}, {C6, 150}, {REST, 50}, {C6, 80}, {E6, 200},
};
inline constexpr size_t SFX_VICTORY_GROWBALL_COUNT = 7;

} // namespace sounds
} // namespace handheld

#endif
