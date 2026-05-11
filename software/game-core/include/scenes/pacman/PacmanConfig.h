#ifndef SCENES_PACMAN_PACMAN_CONFIG_H
#define SCENES_PACMAN_PACMAN_CONFIG_H

#include "core/graphics/Color.h"
#include <cstdint>

namespace handheld {
namespace pacman {
namespace cfg {

// 颜色
constexpr Color WALL_COLOR = rgb565(15, 18, 60);
constexpr Color DOT_COLOR = rgb565(200, 170, 130);
constexpr Color POWER_COLOR = rgb565(220, 190, 150);
constexpr Color PAC_COLOR = rgb565(240, 210, 50);
constexpr Color BG_COLOR = rgb565(2, 2, 10);
constexpr Color GHOST1_COL = rgb565(220, 60, 60);
constexpr Color GHOST2_COL = rgb565(200, 80, 180);
constexpr Color VULN_COLOR = rgb565(50, 50, 170);
constexpr Color VULN_BLINK = rgb565(200, 200, 240);
constexpr Color AURA_COLOR = rgb565(180, 140, 220);
constexpr Color PARTICLE_COLOR = rgb565(240, 210, 80);
constexpr Color WIN_COLOR = rgb565(120, 200, 220);
constexpr Color MAZE_BORDER = rgb565(10, 12, 30);
constexpr Color HINT_COLOR = rgb565(120, 140, 120);
constexpr Color PAUSE_BG = rgb565(4, 6, 20);
constexpr Color PAUSE_TEXT = rgb565(220, 200, 100);

// 布局
constexpr int16_t GRID = 10;
constexpr int16_t CELL = 8;

// 游戏参数 (30 FPS 基准)
constexpr uint32_t PAC_MOVE_INTERVAL = 12;
constexpr uint32_t GHOST_MOVE_INTERVAL = 16;
constexpr uint32_t POWER_DURATION = 240;
constexpr uint32_t DYING_TIMER = 20;
constexpr uint32_t CELEBRATION_DURATION = 30;
constexpr int16_t MAX_DEATH_PARTICLES = 10;
constexpr int8_t INIT_LIVES = 3;

// 分数
constexpr int16_t DOT_SCORE = 10;
constexpr int16_t GHOST_SCORE = 50;

// 初始位置
constexpr int8_t PAC_START_X = 5, PAC_START_Y = 8;
constexpr int8_t GHOST0_X = 4, GHOST0_Y = 4;
constexpr int8_t GHOST1_X = 5, GHOST1_Y = 4;

// 迷宫 (10x10)
constexpr char MAZE[GRID][GRID] = {
    {'W','W','W','W','W','W','W','W','W','W'},
    {'W','.','.','.','.','.','.','.','.','W'},
    {'W','.','W','W','.','W','W','.','.','W'},
    {'W','P','W','.','.','.','W','P','.','W'},
    {'W','.','.','.','W','.','.','.','.','W'},
    {'W','.','.','.','W','.','.','.','.','W'},
    {'W','P','W','.','.','.','W','P','.','W'},
    {'W','.','W','W','.','W','W','.','.','W'},
    {'W','.','.','.','.','.','.','.','.','W'},
    {'W','W','W','W','W','W','W','W','W','W'},
};

}  // namespace cfg
}  // namespace pacman
}  // namespace handheld

#endif
