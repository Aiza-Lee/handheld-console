#ifndef SCENES_SNAKE_SNAKE_CONFIG_H
#define SCENES_SNAKE_SNAKE_CONFIG_H

#include "core/graphics/Color.h"
#include <cstdint>

namespace handheld::snake::cfg {

constexpr bool ENABLE_BGM = true;

// ── 地图 ────────────────────────────────────────
constexpr int16_t GRID_W = 13;
constexpr int16_t GRID_H = 12;
constexpr int16_t CELL = 6;
constexpr int16_t STATUS_H = 7;
constexpr int16_t AREA_W = GRID_W * CELL;
constexpr int16_t AREA_H = GRID_H * CELL;
constexpr int16_t MAX_LENGTH = GRID_W * GRID_H;

// ── 速度（值越小越快，单位：逻辑帧）────
constexpr uint32_t INITIAL_INTERVAL = 8;
constexpr uint32_t MIN_INTERVAL = 3;
constexpr uint8_t SPEED_DIVISOR = 4;   // 每吃 N 个食物速度 +1

// ── 初始长度 ────────────────────────────────────
constexpr int16_t INITIAL_BODY_LENGTH = 3;

// ── 颜色 ────────────────────────────────────────
constexpr Color HEAD_COLOR = rgb565(60, 180, 80);
constexpr Color HEAD_EYE = rgb565(200, 235, 220);
constexpr Color FOOD_COLOR = rgb565(255, 55, 55);
constexpr Color BORDER_COLOR = rgb565(20, 40, 25);
constexpr Color GRID_LINE = rgb565(6, 12, 8);
constexpr Color BG_COLOR = rgb565(3, 8, 4);
constexpr Color BAR_COLOR = rgb565(4, 12, 6);
constexpr Color SCORE_COLOR = rgb565(140, 180, 150);
constexpr Color GAMEOVER_COLOR = rgb565(255, 145, 190);
constexpr Color OVERLAY_BG = rgb565(3, 8, 6);
constexpr Color WIN_COLOR = rgb565(120, 200, 220);
constexpr Color HINT_COLOR = rgb565(120, 140, 120);
constexpr Color PAUSE_BG = rgb565(3, 10, 6);
constexpr Color PAUSE_TEXT = rgb565(100, 200, 140);

// ── 渐变 ────────────────────────────────────────
constexpr uint8_t HEAD_R = 60, HEAD_G = 180, HEAD_B = 80;
constexpr uint8_t TAIL_R = 15, TAIL_G = 60, TAIL_B = 20;

} // namespace handheld::snake::cfg

#endif
