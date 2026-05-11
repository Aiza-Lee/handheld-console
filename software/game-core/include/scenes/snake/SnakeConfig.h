#ifndef SCENES_SNAKE_SNAKE_CONFIG_H
#define SCENES_SNAKE_SNAKE_CONFIG_H

#include "core/graphics/Color.h"
#include <cstdint>



namespace handheld::snake::cfg {

// 颜色
constexpr Color HEAD_COLOR = rgb565(60, 180, 80);
constexpr Color TAIL_COLOR = rgb565(15, 60, 20);
constexpr Color HEAD_EYE = rgb565(200, 235, 220);
constexpr Color FOOD_COLOR = rgb565(255, 145, 190);
constexpr Color GRID_LINE = rgb565(8, 16, 10);
constexpr Color BG_COLOR = rgb565(3, 8, 4);
constexpr Color SCORE_COLOR = rgb565(140, 180, 150);
constexpr Color GAMEOVER_COLOR = rgb565(255, 145, 190);
constexpr Color OVERLAY_BG = rgb565(3, 8, 6);
constexpr Color EAT_PARTICLE = rgb565(200, 180, 100);
constexpr Color WIN_COLOR = rgb565(120, 200, 220);
constexpr Color HINT_COLOR = rgb565(120, 140, 120);
constexpr Color PAUSE_BG = rgb565(3, 10, 6);
constexpr Color PAUSE_TEXT = rgb565(100, 200, 140);

// 布局
constexpr int16_t GRID_SIZE = 10;
constexpr int16_t CELL_SIZE = 8;
constexpr int16_t MAX_LENGTH = 100;

// 游戏参数
constexpr uint32_t INITIAL_MOVE_INTERVAL = 16;
constexpr uint32_t MIN_MOVE_INTERVAL = 6;
constexpr int16_t INITIAL_BODY_LENGTH = 3;
constexpr uint32_t EAT_EFFECT_DURATION = 10;
constexpr uint32_t SHAKE_DURATION = 15;

// 渐变端点 (8-bit RGB)
constexpr uint8_t HEAD_R = 60, HEAD_G = 180, HEAD_B = 80;
constexpr uint8_t TAIL_R = 15, TAIL_G = 60, TAIL_B = 20;

// 动画
constexpr float WAVE_FREQ = 0.5F;
constexpr float WAVE_PHASE_PER_SEG = 0.7F;

// 初始位置
constexpr int8_t INIT_HEAD_X = 2, INIT_HEAD_Y = 5;
constexpr int8_t INIT_SEG2_X = 1, INIT_SEG2_Y = 5;
constexpr int8_t INIT_SEG3_X = 0, INIT_SEG3_Y = 5;

} // namespace handheld::snake::cfg



#endif
