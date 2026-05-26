#ifndef SCENES_BREAKOUT_BREAKOUT_CONFIG_H
#define SCENES_BREAKOUT_BREAKOUT_CONFIG_H

#include "core/graphics/Color.h"
#include <cstdint>

namespace handheld::breakout::cfg {

// 音频
constexpr bool ENABLE_BGM = true;

// ── 颜色 ──────────────────────────────────────
constexpr Color BRICK_COLORS[5] = {
    rgb565(200, 60, 60),
    rgb565(200, 110, 30),
    rgb565(190, 180, 40),
    rgb565(40, 150, 60),
    rgb565(50, 80, 190),
};
constexpr Color INDESTRUCTIBLE_COLOR = rgb565(100, 100, 110);
constexpr Color INDESTRUCTIBLE_EDGE = rgb565(140, 140, 150);
constexpr Color BG_COLOR = rgb565(4, 6, 12);
constexpr Color PADDLE_COLOR = rgb565(130, 160, 220);
constexpr Color PADDLE_EDGE = rgb565(170, 200, 250);
constexpr Color BALL_COLOR = rgb565(220, 240, 255);
constexpr Color TRAIL_COLOR = rgb565(60, 80, 140);
constexpr Color SPARK_COLOR = rgb565(255, 220, 150);
constexpr Color BAR_COLOR = rgb565(6, 10, 18);
constexpr Color WIN_COLOR = rgb565(120, 200, 220);
constexpr Color HINT_COLOR = rgb565(120, 140, 120);
constexpr Color TRIPLE_COLOR = rgb565(255, 210, 60);
constexpr Color SPLIT_COLOR = rgb565(60, 210, 220);
constexpr Color PAUSE_BG = rgb565(5, 8, 18);
constexpr Color PAUSE_TEXT = rgb565(180, 200, 255);

// ── 布局 ──────────────────────────────────────
constexpr int16_t PADDLE_W = 16;
constexpr int16_t PADDLE_H = 4;
constexpr int16_t PADDLE_Y = 74;
constexpr int16_t BALL_SIZE = 1;
constexpr int16_t BRICK_SIZE = 4;
constexpr int16_t BRICK_GAP = 1;
constexpr int16_t BRICK_CELL = BRICK_SIZE + BRICK_GAP;  // 5
constexpr int16_t BRICK_COLS = 15;
constexpr int16_t MAX_BRICK_ROWS = 11;
constexpr int16_t BRICK_OX = 2;  // (80 - 15*5) / 2 = 2
constexpr int16_t BRICK_OY = 8;
constexpr int16_t STATUS_H = 6;

// ── 瓦片缓存尺寸（仅覆盖砖块区域）──
constexpr int16_t TILE_W = BRICK_COLS * BRICK_CELL;        // 75
constexpr int16_t TILE_H = MAX_BRICK_ROWS * BRICK_CELL;    // 55
constexpr uint8_t TILE_EMPTY = 0;
constexpr uint8_t TILE_INDESTRUCTIBLE = 9;

// ── 球的限制 ──────────────────────────────────
constexpr int16_t MAX_BALLS = 16;
constexpr int16_t SPLIT_SOFT_CAP = 11;
constexpr int16_t TRIPLE_RESERVE = 3;

// ── 道具 ──────────────────────────────────────
constexpr int16_t MAX_POWERUPS = 3;
constexpr uint8_t POWERUP_DROP_CHANCE = 20;
constexpr int16_t POWERUP_SIZE = 4;
constexpr int8_t POWERUP_VY = 1;
constexpr uint8_t POWERUP_TYPE_TRIPLE = 0;
constexpr uint8_t POWERUP_TYPE_SPLIT = 1;

// ── 粒子 ──────────────────────────────────────
constexpr int16_t MAX_PARTICLES = 24;
constexpr uint8_t PARTICLE_LIFE = 10;

// ── 特效计时 ──────────────────────────────────
constexpr uint8_t SPARK_DURATION = 4;
constexpr uint32_t CELEBRATION_DURATION = 50;
constexpr int8_t PADDLE_SPEED = 2;

// ── 挡板反弹区域 ──────────────────────────────
constexpr int16_t PADDLE_ZONES[5][2] = {
    {-5, -2}, {-2, -1}, {2, 0}, {5, 1}, {99, 2},
};
constexpr int8_t PADDLE_ZONE_VY[5] = {-1, -2, -2, -2, -1};

// ── 关卡定义 ──────────────────────────────────
constexpr int16_t LEVEL_COUNT = 5;

constexpr uint8_t LEVEL_1[MAX_BRICK_ROWS][BRICK_COLS] = {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
    {3,3,3,3,3,3,3,3,3,3,3,3,3,3,3},
    {4,4,4,4,4,4,4,4,4,4,4,4,4,4,4},
    {5,5,5,5,5,5,5,5,5,5,5,5,5,5,5},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
};

constexpr uint8_t LEVEL_2[MAX_BRICK_ROWS][BRICK_COLS] = {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,1,1,1,1,1,0,1,1,1,1,1,1,1,1},
    {0,2,0,2,0,2,0,2,0,2,0,2,0,2,0},
    {9,0,3,0,3,0,9,0,3,0,3,0,9,0,3},
    {0,4,0,4,0,4,0,4,0,4,0,4,0,4,0},
    {5,0,5,0,5,0,5,0,5,0,5,0,5,0,5},
    {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
};

constexpr uint8_t LEVEL_3[MAX_BRICK_ROWS][BRICK_COLS] = {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {9,9,9,0,0,0,0,0,0,0,0,0,9,9,9},
    {9,1,9,0,1,1,1,1,1,1,1,0,9,1,9},
    {9,1,9,0,1,0,0,0,0,0,1,0,9,1,9},
    {0,1,0,0,1,0,0,0,0,0,1,0,0,1,0},
    {0,1,0,0,1,0,0,0,0,0,1,0,0,1,0},
    {0,1,0,0,1,1,1,1,1,1,1,0,0,1,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
};

constexpr uint8_t LEVEL_4[MAX_BRICK_ROWS][BRICK_COLS] = {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {9,1,9,2,9,3,9,4,9,5,9,1,9,2,9},
    {0,2,0,3,0,4,0,5,0,1,0,2,0,3,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {5,5,5,0,4,4,0,3,3,0,2,2,0,1,1},
    {9,9,9,0,9,9,0,9,9,0,9,9,0,9,9},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,0,1,0,2,0,2,0,3,0,3,0,4,0,4},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
};

constexpr uint8_t LEVEL_5[MAX_BRICK_ROWS][BRICK_COLS] = {
    {9,9,0,0,0,0,0,0,0,0,0,0,0,9,9},
    {9,1,9,0,0,0,0,0,0,0,0,0,9,2,9},
    {0,3,0,9,0,0,0,0,0,0,0,9,0,3,0},
    {0,0,0,0,9,1,1,1,1,1,9,0,0,0,0},
    {0,0,0,0,9,4,4,4,4,4,9,0,0,0,0},
    {0,0,0,9,0,0,0,0,0,0,0,9,0,0,0},
    {0,0,9,0,0,0,0,0,0,0,0,0,9,0,0},
    {0,9,0,0,0,0,0,0,0,0,0,0,0,9,0},
    {9,0,0,0,0,0,0,0,0,0,0,0,0,0,9},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
};

constexpr const uint8_t (*LEVELS[LEVEL_COUNT])[MAX_BRICK_ROWS][BRICK_COLS] = {
    &LEVEL_1, &LEVEL_2, &LEVEL_3, &LEVEL_4, &LEVEL_5,
};

} // namespace handheld::breakout::cfg

#endif
