#ifndef SCENES_MP3_BGM_CATALOG_H
#define SCENES_MP3_BGM_CATALOG_H

// MP3 播放器模块的资源目录：所有非菜单 BGM tone 数组在此集中管理。
// 各游戏屏不再直接 set_bgm(...) 调用自己的 BGM；如有需要，由 MP3 资源层负责调度。
// 当前 BGM_MENU 保留在 core/audio/Sounds.h 由 MenuScreen 直接播放。

#include "core/audio/Sounds.h"

#include <cstddef>
#include <cstdint>

namespace handheld::mp3::bgm {

enum class BgmId : uint8_t {
    GUIDE,
    SETTINGS,
    PLAYGROUND,
    SNAKE,
    PACMAN,
    BREAKOUT,
    INVADERS,
    GROWBALL,
    TETRIS,
    GAME_2048,
    PONG,
    _count,
};

// ── Tone 数组（从 Sounds.h 原样迁移）─────────────────────────────────

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

inline constexpr Tone BGM_INVADERS[] = {
    {  C2, 200},
    {REST, 250},
    { Cs2, 200},
    {REST, 250},
    {  D2, 200},
    {REST, 250},
    { Ds2, 200},
    {REST, 250},
};
inline constexpr size_t BGM_INVADERS_COUNT = 8;

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

// 经典 Korobeiniki 旋律（Tetris A 主题）简化循环，**整体下移一个八度**
inline constexpr Tone BGM_TETRIS[] = {
    {  E4, 400}, {  B3, 200}, {  A3, 200}, {  D4, 400},
    {  C4, 200}, {  B3, 200}, {  A3, 200}, {  G3, 200},
    {REST, 200},
    {  E4, 400}, {  C4, 200}, {  D4, 200}, {  E4, 400},
    {  B3, 200}, {  A3, 200}, {  G3, 200}, {  A3, 200},
    {REST, 200},
    {  D4, 400}, {  A3, 200}, {  G3, 200}, {  E3, 400},
    {  D3, 200}, {  C3, 200}, {  B2, 200}, {  C3, 200},
    {REST, 200},
    {  E3, 400}, {  C3, 200}, {  D3, 200}, {  E3, 400},
    {  A2, 200}, {  G2, 200}, {  E2, 200}, {  G2, 200},
    {REST, 400},
};
inline constexpr size_t BGM_TETRIS_COUNT = 33;

inline constexpr Tone BGM_2048[] = {
    // 上行：C3 → D3 → E3 → F3
    {  C3, 500},
    {  D3, 400},
    {  E3, 500},
    {  F3, 600},
    {REST, 400},

    // 下行：E3 → D3 → C3
    {  E3, 400},
    {  D3, 500},
    {  C3, 700},
    {REST, 500},

    // 中段：D3 → E3 → F3
    {  D3, 500},
    {  E3, 500},
    {  F3, 500},
    {REST, 600},

    // 高点：G3
    {  G3, 600},
    {REST, 600},

    // 收束：F3 → C3
    {  F3, 500},
    {  C3, 700},
    {REST, 800},
};
inline constexpr size_t BGM_2048_COUNT = 18;

// 乒乓球经典 8-bit 比赛音乐（重复循环）
inline constexpr Tone BGM_PONG[] = {
    {  C4, 200},
    {  G3, 200},
    {  C4, 200},
    {REST, 200},
    {  E4, 200},
    {  C4, 200},
    {  G3, 200},
    {REST, 200},
    {  A3, 200},
    {  E4, 200},
    {  A3, 200},
    {REST, 200},
    {  C4, 200},
    {  A3, 200},
    {  E4, 200},
    {REST, 200},
    {  D4, 200},
    {  A3, 200},
    {  D4, 200},
    {REST, 200},
    {  F4, 200},
    {  D4, 200},
    {  A3, 200},
    {REST, 200},
    {  G3, 200},
    {  B3, 200},
    {  D4, 200},
    {  G4, 200},
    {REST, 200},
    {  C4, 300},
    {  G3, 200},
    {REST, 300},
};
inline constexpr size_t BGM_PONG_COUNT = 32;

// ── 目录表 ──────────────────────────────────────────────────────────

struct BgmDef {
    const Tone* tones;
    std::size_t count;
};

inline constexpr BgmDef BGM_CATALOG[] = {
    {BGM_GUIDE,      BGM_GUIDE_COUNT},
    {BGM_SETTINGS,   BGM_SETTINGS_COUNT},
    {BGM_PLAYGROUND, BGM_PLAYGROUND_COUNT},
    {BGM_SNAKE,      BGM_SNAKE_COUNT},
    {BGM_PACMAN,     BGM_PACMAN_COUNT},
    {BGM_BREAKOUT,   BGM_BREAKOUT_COUNT},
    {BGM_INVADERS,   BGM_INVADERS_COUNT},
    {BGM_GROWBALL,   BGM_GROWBALL_COUNT},
    {BGM_TETRIS,     BGM_TETRIS_COUNT},
    {BGM_2048,       BGM_2048_COUNT},
    {BGM_PONG,       BGM_PONG_COUNT},
};

inline constexpr std::size_t BGM_CATALOG_COUNT =
    sizeof(BGM_CATALOG) / sizeof(BGM_CATALOG[0]);

static_assert(BGM_CATALOG_COUNT == static_cast<std::size_t>(BgmId::_count),
              "BGM_CATALOG 与 BgmId 枚举长度不一致");

[[nodiscard]] constexpr const BgmDef& get(BgmId id) {
    return BGM_CATALOG[static_cast<std::size_t>(id)];
}

} // namespace handheld::mp3::bgm

#endif // SCENES_MP3_BGM_CATALOG_H
