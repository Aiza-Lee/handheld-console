#ifndef SCENES_MP3_MP3_TRACKS_H
#define SCENES_MP3_MP3_TRACKS_H

#include "core/audio/Sounds.h"
#include "scenes/mp3/BgmCatalog.h"

namespace handheld::mp3 {

struct TrackDef {
    const char* name;
    const Tone* tones;
    size_t count;
};

// 16 首内置曲目：
//   - 后 11 首为各历史游戏 BGM（来自 BgmCatalog），以游戏名作为 track 名以便在
//     MP3 播放器中作为"纪念"播放 — 那些游戏本身已不再用 BGM（数据孤岛在此复活）
inline constexpr TrackDef TRACKS[] = {
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
