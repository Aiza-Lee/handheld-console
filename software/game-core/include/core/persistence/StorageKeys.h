#ifndef CORE_PERSISTENCE_STORAGE_KEYS_H
#define CORE_PERSISTENCE_STORAGE_KEYS_H

#include "platform/interfaces/IStorage.h"
#include <cstdint>

namespace handheld::storage {

// ── 容量合约（与 IStorage.h 同步）──
static_assert(handheld::IStorage::MAX_KEYS == 256, "StorageKeys layout assumes 256-key cap");
static_assert(handheld::IStorage::MAX_VALUE_BYTES == 16, "StorageKeys layout assumes 16-byte value cap");

// ── 设置键（0x0001..0x00FF）──────────────────────────
constexpr uint16_t KEY_BGM_VOLUME    = 0x0001;  // uint8_t 0-100
constexpr uint16_t KEY_SFX_VOLUME    = 0x0002;  // uint8_t 0-100
constexpr uint16_t KEY_BACKLIGHT_PCT = 0x0003;  // 预留：uint8_t
constexpr uint16_t KEY_LAST_SCREEN   = 0x0004;  // 预留：uint8_t (ScreenType)

// ── 高分键（0x0100..0x01FF，256 个游戏槽位）──────────
//   key = KEY_HIGH_SCORE_BASE + (game_id & 0x00FF)
//   值固定 4 字节 uint32_t
constexpr uint16_t KEY_HIGH_SCORE_BASE = 0x0100;
constexpr uint16_t key_for_high_score(uint16_t game_id) {
    return static_cast<uint16_t>(KEY_HIGH_SCORE_BASE + (game_id & 0x00FF));
}

// ── 默认值 ─────────────────────────────────────────
constexpr uint8_t DEFAULT_BGM_VOLUME = 5;
constexpr uint8_t DEFAULT_SFX_VOLUME = 10;

} // namespace handheld::storage

#endif
