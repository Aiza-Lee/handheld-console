#ifndef SCENES_BREAKOUT_BREAKOUT_CONFIG_H
#define SCENES_BREAKOUT_BREAKOUT_CONFIG_H

#include "core/graphics/Color.h"
#include <cstdint>

namespace handheld {
namespace breakout {
namespace cfg {

// 颜色
constexpr Color BRICK_COLORS[5] = {
    rgb565(200, 60, 60),    // 行 0: 红
    rgb565(200, 110, 30),   // 行 1: 橙
    rgb565(190, 180, 40),   // 行 2: 黄
    rgb565(40, 150, 60),    // 行 3: 绿
    rgb565(50, 80, 190),    // 行 4: 蓝
};
constexpr Color PARTICLE_COLORS[5] = {
    rgb565(220, 100, 80),
    rgb565(220, 140, 60),
    rgb565(200, 190, 70),
    rgb565(70, 170, 90),
    rgb565(80, 110, 210),
};
constexpr Color BG_COLOR = rgb565(4, 6, 12);
constexpr Color PADDLE_COLOR = rgb565(130, 160, 220);
constexpr Color PADDLE_EDGE = rgb565(170, 200, 250);
constexpr Color BALL_COLOR = rgb565(220, 240, 255);
constexpr Color BAR_COLOR = rgb565(6, 10, 18);
constexpr Color TRAIL_COLOR = rgb565(80, 100, 160);
constexpr Color SPARK_COLOR = rgb565(255, 220, 150);
constexpr Color WIN_COLOR = rgb565(120, 200, 220);
constexpr Color HINT_COLOR = rgb565(120, 140, 120);
constexpr Color PAUSE_BG = rgb565(5, 8, 18);
constexpr Color PAUSE_TEXT = rgb565(180, 200, 255);

// 布局
constexpr int16_t PADDLE_W = 16;
constexpr int16_t PADDLE_H = 4;
constexpr int16_t PADDLE_Y = 74;
constexpr int16_t BALL_W = 2;
constexpr int16_t BALL_H = 2;
constexpr int16_t BRICK_ROWS = 5;
constexpr int16_t BRICKS_PER_ROW = 8;
constexpr int16_t BRICK_W = 8;
constexpr int16_t BRICK_H = 5;
constexpr int16_t BRICK_GAP_X = 2;
constexpr int16_t BRICK_GAP_Y = 1;
constexpr int16_t BRICK_OY = 6;

// 分数
constexpr int16_t BRICK_POINTS[5] = {50, 40, 30, 20, 10};

// 游戏参数
constexpr int16_t MAX_BRICK_PARTICLES = 16;
constexpr int8_t INIT_LIVES = 3;
constexpr uint32_t DYING_TIMER = 15;
constexpr uint32_t SHAKE_DURATION = 12;
constexpr uint32_t HIT_SPARK_DURATION = 6;
constexpr uint32_t CELEBRATION_DURATION = 40;

// 球拍碰撞区域映射 (offset, vx, vy)
constexpr int16_t PADDLE_ZONES[5][2] = {
    {-5, -2}, {-2, -1}, {2, 0}, {5, 1}, {99, 2},
};
constexpr int8_t PADDLE_ZONE_VY[5] = {-1, -2, -2, -2, -1};

}  // namespace cfg
}  // namespace breakout
}  // namespace handheld

#endif
