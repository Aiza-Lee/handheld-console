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

    // Octave 2
    C2 = 65,
    Cs2 = 69,
    D2 = 73,
    Ds2 = 78,
    E2 = 82,
    F2 = 87,
    Fs2 = 92,
    G2 = 98,
    Gs2 = 104,
    A2 = 110,
    As2 = 116,
    B2 = 123,

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
    {REST, 500},
    {  C3, 100},
    {  E3, 100},
    {  G3, 100},
    {  C4, 150},
    {REST, 100},
    {  C4,  80},
    {  G3,  80},
    {  E3,  80},
    {  C3, 300},
};
inline constexpr size_t BOOT_COUNT = 10;

inline constexpr Tone BGM_MENU[] = {
    {  C4, 400},
    {  E4, 300},
    {  G4, 400},
    {  A4, 300},
    {  G4, 500},
    {REST, 400},
    {  E4, 300},
    {  G4, 300},
    {  A4, 400},
    {  C5, 300},
    {  A4, 500},
    {REST, 400},
    {  D4, 400},
    {  F4, 300},
    {  A4, 400},
    {  G4, 300},
    {  F4, 400},
    {  E4, 500},
    {REST, 400},
    {  C5, 400},
    {  A4, 300},
    {  G4, 400},
    {  E4, 300},
    {  D4, 400},
    {  C4, 600},
    {REST, 600},
};
inline constexpr size_t BGM_MENU_COUNT = 26;

inline constexpr Tone BGM_GUIDE[] = {
    {  C4, 500},
    {  G4, 400},
    {  E4, 500},
    {  C5, 400},
    {  G4, 500},
    {REST, 350},
    {  F4, 400},
    {  A4, 300},
    {  C5, 400},
    {  A4, 300},
    {  F4, 500},
    {REST, 350},
    {  E4, 400},
    {  G4, 300},
    {  C5, 400},
    {  G4, 300},
    {  E4, 500},
    {  C4, 700},
    {REST, 700},
};
inline constexpr size_t BGM_GUIDE_COUNT = 19;

inline constexpr Tone SFX_SELECT[] = {
    {C4, 30},
};
inline constexpr size_t SFX_SELECT_COUNT = 1;

inline constexpr Tone SFX_CONFIRM[] = {
    {C4, 50},
    {E4, 50},
    {G4, 80},
};
inline constexpr size_t SFX_CONFIRM_COUNT = 3;

// 设置界面低频环境音
inline constexpr Tone BGM_SETTINGS[] = {
    {  C3,  600},
    {REST,  900},
    {  G3,  600},
    {REST,  900},
    {  E3,  600},
    {REST,  900},
    {  D3,  600},
    {REST,  900},
    {  C3,  600},
    {REST, 1200},
};
inline constexpr size_t BGM_SETTINGS_COUNT = 10;

inline constexpr Tone SFX_BACK[] = {
    {C4, 40},
    {G3, 60},
};
inline constexpr size_t SFX_BACK_COUNT = 2;

// ── Playground ────────────────────────────────────────────────────

inline constexpr Tone BGM_PLAYGROUND[] = {
    {  C4, 350},
    {  D4, 250},
    {  G4, 400},
    {  E4, 250},
    {  C5, 400},
    {  G4, 350},
    {  E4, 500},
    {REST, 350},
    {  A3, 300},
    {  C4, 250},
    {  E4, 350},
    {  G4, 300},
    {  D4, 250},
    {  C4, 350},
    {  G3, 600},
    {REST, 400},
    {  F4, 350},
    {  A4, 250},
    {  C5, 400},
    {  G4, 300},
    {  E4, 350},
    {  D4, 250},
    {  C4, 500},
    {REST, 400},
    {  G4, 300},
    {  E4, 250},
    {  C5, 350},
    {  A4, 300},
    {  G4, 250},
    {  E4, 350},
    {  C4, 600},
    {REST, 600},
};
inline constexpr size_t BGM_PLAYGROUND_COUNT = 32;

// ── Snake ─────────────────────────────────────────────────────────

inline constexpr Tone BGM_SNAKE[] = {
    {  B3, 250},
    {  D4, 200},
    {  E4, 300},
    {  D4, 200},
    {  G4, 350},
    {  E4, 250},
    {  D4, 300},
    {REST, 300},
    {  B3, 250},
    {  D4, 200},
    {  E4, 300},
    {  G4, 250},
    {  A4, 300},
    {  G4, 250},
    {  E4, 350},
    {REST, 300},
    {  B2, 350},
    {  D3, 250},
    {  G3, 400},
    {  A3, 250},
    {  B3, 350},
    {  D4, 300},
    {  G4, 400},
    {REST, 350},
    {  D4, 250},
    {  B3, 200},
    {  G4, 300},
    {  E4, 250},
    {  D4, 300},
    {  B3, 350},
    {  G4, 500},
    {REST, 500},
};
inline constexpr size_t BGM_SNAKE_COUNT = 32;

inline constexpr Tone SFX_EAT[] = {
    {C4, 30},
    {E4, 30},
};
inline constexpr size_t SFX_EAT_COUNT = 2;

inline constexpr Tone SFX_SPEED_UP[] = {
    {E4, 40},
    {G4, 40},
    {C5, 60},
};
inline constexpr size_t SFX_SPEED_UP_COUNT = 3;

inline constexpr Tone SFX_WIN[] = {
    {C4,  80},
    {E4,  80},
    {G4, 100},
    {C5, 150},
};
inline constexpr size_t SFX_WIN_COUNT = 4;

inline constexpr Tone SFX_DEATH_SNAKE[] = {
    {E4, 100},
    {C4, 100},
    {A3, 150},
    {E3, 200},
};
inline constexpr size_t SFX_DEATH_SNAKE_COUNT = 4;

// ── Pac-Man ───────────────────────────────────────────────────────

inline constexpr Tone BGM_PACMAN[] = {
    {  C5, 300},
    {  E5, 250},
    {  G5, 350},
    {  C6, 300},
    {  G5, 250},
    {  E5, 300},
    {  C5, 400},
    {REST, 300},
    {  A4, 300},
    {  C5, 250},
    {  E5, 350},
    {  A5, 300},
    {  E5, 250},
    {  C5, 300},
    {  A4, 400},
    {REST, 300},
    {  F4, 350},
    {  A4, 250},
    {  C5, 400},
    {  F5, 300},
    {  E5, 250},
    {  C5, 300},
    {  G4, 400},
    {REST, 350},
    {  E4, 300},
    {  G4, 250},
    {  C5, 350},
    {  E5, 300},
    {  C5, 400},
    {  G4, 500},
    {  C4, 700},
    {REST, 600},
};
inline constexpr size_t BGM_PACMAN_COUNT = 32;

inline constexpr Tone SFX_CHOMP[] = {
    {  E4, 40},
    {REST, 15},
    {  C4, 40},
    {REST, 15},
    {  E4, 40},
    {REST, 15},
    {  C4, 40},
    {REST, 15},
};
inline constexpr size_t SFX_CHOMP_COUNT = 8;

inline constexpr Tone SFX_DEATH_PACMAN[] = {
    {B4, 100},
    {A4,  80},
    {G4,  80},
    {F4,  80},
    {E4, 100},
    {D4, 100},
    {C4, 150},
    {G3, 200},
    {C3, 300},
};
inline constexpr size_t SFX_DEATH_PACMAN_COUNT = 9;

inline constexpr Tone SFX_POWER_PELLET[] = {
    {C4,  40},
    {E4,  40},
    {G4,  40},
    {C5,  60},
    {E5,  60},
    {G5,  80},
    {C6, 100},
};
inline constexpr size_t SFX_POWER_PELLET_COUNT = 7;

inline constexpr Tone SFX_GHOST_EATEN[] = {
    {C4, 30},
    {E4, 30},
    {G4, 40},
    {C5, 50},
    {E5, 30},
};
inline constexpr size_t SFX_GHOST_EATEN_COUNT = 5;

inline constexpr Tone SFX_LEVEL_UP[] = {
    {  C4,  60},
    {  E4,  60},
    {  G4,  80},
    {  C5, 120},
    {REST,  60},
    {  C5,  60},
    {  E5,  80},
    {  G5, 150},
};
inline constexpr size_t SFX_LEVEL_UP_COUNT = 8;

// ── Breakout ──────────────────────────────────────────────────────

inline constexpr Tone BGM_BREAKOUT[] = {
    {  G4, 300},
    {  C5, 250},
    {  G4, 300},
    {  D5, 350},
    {  C5, 300},
    {  G4, 250},
    {  E5, 400},
    {REST, 300},
    {  D5, 300},
    {  C5, 250},
    {  A4, 350},
    {  G4, 250},
    {  E4, 350},
    {  D4, 250},
    {  C4, 400},
    {REST, 350},
    {  E4, 350},
    {  G4, 250},
    {  C5, 400},
    {  E5, 300},
    {  D5, 250},
    {  C5, 350},
    {  G4, 450},
    {REST, 350},
    {  D4, 400},
    {  G4, 300},
    {  C5, 400},
    {  G4, 350},
    {  E4, 400},
    {  C4, 600},
    {REST, 600},
};
inline constexpr size_t BGM_BREAKOUT_COUNT = 31;

inline constexpr Tone SFX_BRICK[] = {
    {C5, 25},
    {E5, 15},
};
inline constexpr size_t SFX_BRICK_COUNT = 2;

inline constexpr Tone SFX_PADDLE[] = {
    {G4, 25},
    {C5, 15},
};
inline constexpr size_t SFX_PADDLE_COUNT = 2;

inline constexpr Tone SFX_WALL_BOUNCE[] = {
    {G4, 15},
};
inline constexpr size_t SFX_WALL_BOUNCE_COUNT = 1;

inline constexpr Tone SFX_BALL_LOST[] = {
    {G3,  80},
    {C3, 120},
};
inline constexpr size_t SFX_BALL_LOST_COUNT = 2;

inline constexpr Tone SFX_POWERUP[] = {
    {C4, 30},
    {E4, 30},
    {G4, 40},
    {C5, 50},
};
inline constexpr size_t SFX_POWERUP_COUNT = 4;

inline constexpr Tone SFX_INDESTRUCTIBLE[] = {
    {G3, 20},
};
inline constexpr size_t SFX_INDESTRUCTIBLE_COUNT = 1;

inline constexpr Tone SFX_LEVEL_CLEAR_BREAKOUT[] = {
    {  C4,  50},
    {  E4,  50},
    {  G4,  70},
    {  C5, 100},
    {REST,  40},
    {  C5,  50},
    {  E5,  70},
    {  G5, 120},
};
inline constexpr size_t SFX_LEVEL_CLEAR_BREAKOUT_COUNT = 8;

inline constexpr Tone SFX_DEATH_BREAKOUT[] = {
    {E4, 100},
    {C4, 100},
    {G3, 150},
    {E3, 200},
};
inline constexpr size_t SFX_DEATH_BREAKOUT_COUNT = 4;

// ── Invaders ──────────────────────────────────────────────────────

inline constexpr Tone BGM_INVADERS[] = {
    {  C2, 150},
    {REST, 150},
    { Cs2, 150},
    {REST, 150},
    {  D2, 150},
    {REST, 150},
    { Ds2, 150},
    {REST, 150},
};
inline constexpr size_t BGM_INVADERS_COUNT = 8;

inline constexpr Tone SFX_SHOOT[] = {
    {C5, 30},
};
inline constexpr size_t SFX_SHOOT_COUNT = 1;

inline constexpr Tone SFX_ENEMY_SHOOT[] = {
    {E4, 25},
    {C4, 20},
};
inline constexpr size_t SFX_ENEMY_SHOOT_COUNT = 2;

inline constexpr Tone SFX_PLAYER_HIT[] = {
    {G3,  60},
    {E3,  60},
    {C3, 100},
};
inline constexpr size_t SFX_PLAYER_HIT_COUNT = 3;

inline constexpr Tone SFX_SAUCER_APPEAR[] = {
    {C4, 40},
    {E4, 40},
    {G4, 60},
    {E4, 40},
    {C4, 60},
};
inline constexpr size_t SFX_SAUCER_APPEAR_COUNT = 5;

inline constexpr Tone SFX_LEVEL_CLEAR_INVADERS[] = {
    {  C4,  50},
    {  E4,  50},
    {  G4,  70},
    {  C5, 100},
    {REST,  40},
    {  G4,  60},
    {  C5,  80},
    {  E5, 120},
};
inline constexpr size_t SFX_LEVEL_CLEAR_INVADERS_COUNT = 8;

inline constexpr Tone SFX_EXPLOSION[] = {
    {  C4, 50},
    {  G3, 40},
    {  C3, 60},
    {REST, 20},
    {  G3, 40},
};
inline constexpr size_t SFX_EXPLOSION_COUNT = 5;

// ── Growball ──────────────────────────────────────────────────────

inline constexpr Tone BGM_GROWBALL[] = {
    {  C4, 150},
    {  E4, 150},
    {  G4, 200},
    {  C5, 300},
    {REST, 100},
    {  E4, 150},
    {  G4, 150},
    {  C5, 200},
    {  E5, 400},
    {REST, 200},
    {  G4, 150},
    {  E4, 150},
    {  C4, 200},
    {  G3, 400},
    {REST, 300},
};
inline constexpr size_t BGM_GROWBALL_COUNT = 15;

inline constexpr Tone SFX_EAT_GROWBALL[] = {
    {C4, 30},
    {E4, 40},
    {G4, 30},
};
inline constexpr size_t SFX_EAT_GROWBALL_COUNT = 3;

inline constexpr Tone SFX_EAT_ENEMY_GROWBALL[] = {
    {  C3,  40},
    {  E3,  40},
    {  G3,  50},
    {  C4,  60},
    {  E4,  80},
    {  G4, 100},
    {REST,  30},
};
inline constexpr size_t SFX_EAT_ENEMY_GROWBALL_COUNT = 7;

inline constexpr Tone SFX_DEATH_GROWBALL[] = {
    {  E4, 100},
    {  C4, 100},
    {  A3, 150},
    {  E3, 200},
    {REST, 100},
    {  C3, 300},
};
inline constexpr size_t SFX_DEATH_GROWBALL_COUNT = 6;

inline constexpr Tone SFX_VICTORY_GROWBALL[] = {
    {  C4,  80},
    {  E4,  80},
    {  G4,  80},
    {  C5, 150},
    {REST,  50},
    {  C5,  80},
    {  E5, 200},
};
inline constexpr size_t SFX_VICTORY_GROWBALL_COUNT = 7;

inline constexpr Tone SFX_THRUST[] = {
    {C3, 25},
};
inline constexpr size_t SFX_THRUST_COUNT = 1;

inline constexpr Tone SFX_AI_GROW[] = {
    {G3, 25},
    {C4, 20},
};
inline constexpr size_t SFX_AI_GROW_COUNT = 2;

// ── Playground ────────────────────────────────────────────────────

inline constexpr Tone SFX_MODE_SWITCH[] = {
    {C4, 40},
    {E4, 30},
    {G4, 50},
};
inline constexpr size_t SFX_MODE_SWITCH_COUNT = 3;

inline constexpr Tone SFX_TOGGLE_ON[] = {
    {C4, 30},
    {G4, 20},
};
inline constexpr size_t SFX_TOGGLE_ON_COUNT = 2;

inline constexpr Tone SFX_TOGGLE_OFF[] = {
    {G4, 30},
    {C4, 20},
};
inline constexpr size_t SFX_TOGGLE_OFF_COUNT = 2;

// ── Tetris ────────────────────────────────────────────────────────

// 经典 Korobeiniki 旋律（Tetris A 主题）简化循环
inline constexpr Tone BGM_TETRIS[] = {
    {  E5, 400}, {  B4, 200}, {  A4, 200}, {  D5, 400},
    {  C5, 200}, {  B4, 200}, {  A4, 200}, {  G4, 200},
    {REST, 200},
    {  E5, 400}, {  C5, 200}, {  D5, 200}, {  E5, 400},
    {  B4, 200}, {  A4, 200}, {  G4, 200}, {  A4, 200},
    {REST, 200},
    {  D5, 400}, {  A4, 200}, {  G4, 200}, {  E4, 400},
    {  D4, 200}, {  C4, 200}, {  B3, 200}, {  C4, 200},
    {REST, 200},
    {  E4, 400}, {  C4, 200}, {  D4, 200}, {  E4, 400},
    {  A3, 200}, {  G3, 200}, {  E3, 200}, {  G3, 200},
    {REST, 400},
};
inline constexpr size_t BGM_TETRIS_COUNT = 33;

inline constexpr Tone SFX_TETRIS_MOVE[] = {
    {C5, 30},
};
inline constexpr size_t SFX_TETRIS_MOVE_COUNT = 1;

inline constexpr Tone SFX_TETRIS_DROP[] = {
    {A3, 50},
};
inline constexpr size_t SFX_TETRIS_DROP_COUNT = 1;

inline constexpr Tone SFX_TETRIS_CLEAR[] = {
    {E4, 60},
    {G4, 60},
    {C5, 80},
};
inline constexpr size_t SFX_TETRIS_CLEAR_COUNT = 3;

inline constexpr Tone SFX_TETRIS_GAMEOVER[] = {
    {A3, 150},
    {F3, 150},
    {D3, 300},
};
inline constexpr size_t SFX_TETRIS_GAMEOVER_COUNT = 3;

// ── 2048 ──────────────────────────────────────────────────────────

inline constexpr Tone BGM_2048[] = {
    {  C4, 300},
    {  E4, 200},
    {  G4, 300},
    {  B4, 200},
    {  C5, 400},
    {  B4, 200},
    {  G4, 200},
    {  E4, 300},
    {REST, 200},
    {  A3, 300},
    {  C4, 200},
    {  E4, 300},
    {  G4, 200},
    {  A4, 300},
    {  G4, 200},
    {  E4, 300},
    {  D4, 200},
    {REST, 300},
};
inline constexpr size_t BGM_2048_COUNT = 18;

inline constexpr Tone SFX_2048_MOVE[] = {
    {C4, 40},
};
inline constexpr size_t SFX_2048_MOVE_COUNT = 1;

inline constexpr Tone SFX_2048_MERGE[] = {
    {E4, 40},
    {G4, 50},
};
inline constexpr size_t SFX_2048_MERGE_COUNT = 2;

inline constexpr Tone SFX_2048_GAMEOVER[] = {
    {A3, 200},
    {F3, 200},
    {D3, 400},
};
inline constexpr size_t SFX_2048_GAMEOVER_COUNT = 3;

// ── Pong ──────────────────────────────────────────────────────────

inline constexpr Tone BGM_PONG[] = {
    {REST, 1000},
};
inline constexpr size_t BGM_PONG_COUNT = 1;

inline constexpr Tone SFX_PONG_HIT[] = {
    {A4, 30},
};
inline constexpr size_t SFX_PONG_HIT_COUNT = 1;

inline constexpr Tone SFX_PONG_SCORE[] = {
    {E4, 80},
    {C4, 80},
};
inline constexpr size_t SFX_PONG_SCORE_COUNT = 2;

inline constexpr Tone SFX_PONG_WIN[] = {
    {C4, 100},
    {E4, 100},
    {G4, 100},
    {C5, 200},
};
inline constexpr size_t SFX_PONG_WIN_COUNT = 4;

} // namespace sounds
} // namespace handheld

#endif
