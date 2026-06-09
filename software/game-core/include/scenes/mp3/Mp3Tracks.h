#ifndef SCENES_MP3_MP3_TRACKS_H
#define SCENES_MP3_MP3_TRACKS_H

#include "core/audio/Sounds.h"
#include "scenes/mp3/BgmCatalog.h"

namespace handheld::mp3 {

// 5 首内置曲（每首 ~16-24 个 Tone 短旋律；与全局 Sounds.h 的 BGM_* 解耦）
// 用 Note 枚举值（C2..C7），整体偏安静，作为场景背景音

inline constexpr Tone TRACK_1[] = {
    {  C4, 400}, {  E4, 300}, {  G4, 400}, {  C5, 600},
    {  B4, 300}, {  G4, 300}, {  E4, 400}, {  C4, 600},
    {  D4, 300}, {  F4, 300}, {  A4, 400}, {  D5, 600},
    {  C5, 400}, {  A4, 300}, {  F4, 300}, {  E4, 400}, {  C4, 800},
};

inline constexpr Tone TRACK_2[] = {
    {  A3, 600}, {REST, 200}, {  A3, 400},
    {  C4, 400}, {  B3, 400}, {  A3, 400}, {  G3, 600},
    {REST, 200}, {  E3, 400}, {  F3, 400}, {  G3, 400}, {  A3, 800},
    {REST, 400},
};

inline constexpr Tone TRACK_3[] = {
    {  C4, 200}, {  E4, 200}, {REST, 100}, {  C4, 200}, {  E4, 200},
    {  C4, 200}, {  E4, 200}, {REST, 100}, {  G4, 400},
    {  A3, 200}, {  C4, 200}, {REST, 100}, {  A3, 200}, {  C4, 200},
    {  A3, 200}, {  C4, 200}, {REST, 100}, {  E4, 400},
};

inline constexpr Tone TRACK_4[] = {
    {  C4, 300}, {  D4, 300}, {  E4, 300}, {  G4, 600},
    {  E4, 300}, {  D4, 300}, {  C4, 300}, {  A3, 600},
    {  D4, 300}, {  E4, 300}, {  F4, 300}, {  A4, 600},
    {  F4, 300}, {  E4, 300}, {  D4, 300}, {  C4, 600},
};

inline constexpr Tone TRACK_5[] = {
    {  C4, 300}, {  E4, 300}, {  G4, 300}, {  C5, 600},
    {  C4, 300}, {  E4, 300}, {  G4, 300}, {  C5, 600},
    {  F4, 300}, {  A4, 300}, {  C5, 300}, {  F5, 600},
    {  F4, 300}, {  A4, 300}, {  C5, 300}, {  F5, 600},
};

struct TrackDef {
    const char* name;
    const Tone* tones;
    size_t count;
};

// 16 首内置曲目：
//   - 前 5 首为通用 TRACK 01..05（短旋律，主要作为场景静音占位）
//   - 后 11 首为各历史游戏 BGM（来自 BgmCatalog），以游戏名作为 track 名以便在
//     MP3 播放器中作为"纪念"播放 — 那些游戏本身已不再用 BGM（数据孤岛在此复活）
inline constexpr TrackDef TRACKS[] = {
    {"TRACK 01",  TRACK_1, sizeof(TRACK_1)  / sizeof(Tone)},
    {"TRACK 02",  TRACK_2, sizeof(TRACK_2)  / sizeof(Tone)},
    {"TRACK 03",  TRACK_3, sizeof(TRACK_3)  / sizeof(Tone)},
    {"TRACK 04",  TRACK_4, sizeof(TRACK_4)  / sizeof(Tone)},
    {"TRACK 05",  TRACK_5, sizeof(TRACK_5)  / sizeof(Tone)},
    {"SNAKE",     mp3::bgm::BGM_SNAKE,      mp3::bgm::BGM_SNAKE_COUNT},
    {"PACMAN",    mp3::bgm::BGM_PACMAN,     mp3::bgm::BGM_PACMAN_COUNT},
    {"BREAKOUT",  mp3::bgm::BGM_BREAKOUT,   mp3::bgm::BGM_BREAKOUT_COUNT},
    {"INVADERS",  mp3::bgm::BGM_INVADERS,   mp3::bgm::BGM_INVADERS_COUNT},
    {"GROWBALL",  mp3::bgm::BGM_GROWBALL,   mp3::bgm::BGM_GROWBALL_COUNT},
    {"TETRIS",    mp3::bgm::BGM_TETRIS,     mp3::bgm::BGM_TETRIS_COUNT},
    {"2048",      mp3::bgm::BGM_2048,       mp3::bgm::BGM_2048_COUNT},
    {"PONG",      mp3::bgm::BGM_PONG,       mp3::bgm::BGM_PONG_COUNT},
    {"GUIDE",     mp3::bgm::BGM_GUIDE,      mp3::bgm::BGM_GUIDE_COUNT},
    {"PLAYGROUND",mp3::bgm::BGM_PLAYGROUND, mp3::bgm::BGM_PLAYGROUND_COUNT},
    {"SETTINGS",  mp3::bgm::BGM_SETTINGS,   mp3::bgm::BGM_SETTINGS_COUNT},
};
inline constexpr size_t TRACK_COUNT = sizeof(TRACKS) / sizeof(TRACKS[0]);

} // namespace handheld::mp3

#endif
