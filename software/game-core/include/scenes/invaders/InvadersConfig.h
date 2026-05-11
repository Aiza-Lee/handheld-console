#ifndef SCENES_INVADERS_INVADERS_CONFIG_H
#define SCENES_INVADERS_INVADERS_CONFIG_H

#include "core/graphics/Color.h"
#include <cstdint>



namespace handheld::invaders::cfg {

// 颜色
constexpr Color INV_BG = rgb565(4, 8, 20);
constexpr Color INV_PLAYER = rgb565(100, 200, 220);
constexpr Color INV_PLAYER_ENGINE = rgb565(200, 160, 120);
constexpr Color INV_BULLET = rgb565(220, 240, 255);
constexpr Color INV_E_ROW0 = rgb565(180, 80, 120);
constexpr Color INV_E_ROW1 = rgb565(140, 100, 200);
constexpr Color INV_E_ROW2 = rgb565(80, 160, 100);
constexpr Color INV_E_ROW3 = rgb565(200, 140, 60);
constexpr Color INV_EBULLET = rgb565(255, 145, 190);
constexpr Color INV_EXPLOSION = rgb565(255, 200, 100);
constexpr Color INV_STATUS_BG = rgb565(6, 14, 30);
constexpr Color INV_GROUND = rgb565(20, 30, 50);
constexpr Color INV_TEXT = rgb565(180, 200, 230);
constexpr Color INV_LIFE_COLOR = rgb565(100, 200, 220);
constexpr Color INV_WARN_COLOR = rgb565(255, 160, 180);
constexpr Color INV_HINT_COLOR = rgb565(120, 140, 180);
constexpr Color PAUSE_BG = rgb565(5, 10, 25);
constexpr Color PAUSE_TEXT = rgb565(150, 210, 230);

// 布局
constexpr int16_t GRID_COLS = 6;
constexpr int16_t GRID_ROWS = 4;
constexpr int16_t ENEMY_W = 6;
constexpr int16_t ENEMY_H = 5;
constexpr int16_t CELL_W = 7;
constexpr int16_t CELL_H = 8;
constexpr int16_t GRID_OX = 19;
constexpr int16_t GRID_OY = 12;
constexpr int16_t PLAYER_W = 7;
constexpr int16_t PLAYER_H = 5;
constexpr int16_t PLAYER_Y = 72;
constexpr int16_t STATUS_H = 8;
constexpr int16_t MAX_PARTICLES = 12;
constexpr int16_t MAX_EBULLETS = 2;

// 游戏参数 (30 FPS 基准)
constexpr uint32_t INIT_MOVE_INTERVAL = 36;
constexpr uint32_t MIN_MOVE_INTERVAL = 6;
constexpr int8_t INIT_LIVES = 3;
constexpr int16_t PLAYER_SPEED = 2;
constexpr int16_t ENEMY_DROP = 5;
constexpr int16_t PLAYER_START_X = 36;
constexpr uint32_t DYING_TIMER = 20;

// 分数
constexpr int16_t ROW_SCORES[4] = {40, 30, 20, 10};

// 敌人形状 (6x5, 每行两个动画帧)
constexpr const char* ENEMY_SHAPES[4][2] = {
    {
        " X  X "
        "  XX  "
        " XXXX "
        "X XX X"
        "X XX X",
        " X  X "
        " XXXX "
        " XXXX "
        "X XX X"
        " XX XX",
    },
    {
        "X    X"
        " XX XX"
        " XXXX "
        " XX XX"
        "X XX X",
        "X    X"
        "XX  XX"
        " XXXX "
        "X XX X"
        " X X X",
    },
    {
        "  XX  "
        " XXXX "
        "XXXXXX"
        "X XX X"
        "X XX X",
        " XXXX "
        " XXXX "
        "XXXXXX"
        "X XX X"
        "X XX X",
    },
    {
        "  XX  "
        " XXXX "
        "XXXXXX"
        "X XX X"
        "X XX X",
        " XXXX "
        " XXXX "
        "XXXXXX"
        "X XX X"
        "X XX X",
    },
};

} // namespace handheld::invaders::cfg



#endif
