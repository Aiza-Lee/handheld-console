#ifndef SCENES_GROWBALL_GROW_BALL_CONFIG_H
#define SCENES_GROWBALL_GROW_BALL_CONFIG_H

#include "core/graphics/Color.h"
#include <cstdint>

namespace handheld::growball::cfg {

// 世界
constexpr int16_t WORLD_W = 200;
constexpr int16_t WORLD_H = 200;

// 玩家
constexpr float PLAYER_INIT_RADIUS = 4.0F;
constexpr float PLAYER_MAX_RADIUS = 30.0F;
constexpr float PLAYER_THRUST = 0.25F;
constexpr float PLAYER_FRICTION = 0.94F;
constexpr float PLAYER_MAX_SPEED = 1.8F;
constexpr Color PLAYER_COLOR = rgb565(80, 180, 240);

// 食物
constexpr int16_t FOOD_COUNT = 20;
constexpr float FOOD_MIN_RADIUS = 1.0F;
constexpr float FOOD_MAX_RADIUS = 2.5F;
constexpr float FOOD_GROWTH = 0.3F;
constexpr Color FOOD_COLOR = rgb565(220, 200, 100);

// AI
constexpr int16_t AI_COUNT = 4;
constexpr float AI_MIN_RADIUS = 3.0F;
constexpr float AI_MAX_RADIUS = 10.0F;
constexpr float AI_THRUST = 0.15F;
constexpr float AI_FRICTION = 0.95F;
constexpr float AI_MAX_SPEED = 1.2F;
constexpr float AI_WANDER_CHANGE_MS = 2000.0F;
constexpr float AI_EAT_RATIO = 0.4F;

// AI 颜色
constexpr Color AI_COLORS[] = {
	rgb565(240, 100, 80),
	rgb565(100, 220, 100),
	rgb565(220, 140, 60),
	rgb565(180, 100, 220),
	rgb565(240, 200, 80),
};

// 摄像机
constexpr float ZOOM_MIN = 0.35F;
constexpr float ZOOM_MAX = 1.0F;
constexpr float ZOOM_BASE = 80.0F;
constexpr float ZOOM_DIVISOR = 5.0F;

// 背景
constexpr Color BG_COLOR = rgb565(8, 12, 28);
constexpr Color GRID_COLOR = rgb565(14, 20, 42);

// 游戏结束
constexpr Color GAME_OVER_BG = rgb565(0, 0, 0);
constexpr Color GAME_OVER_TEXT = rgb565(240, 80, 80);

}  // namespace handheld::growball::cfg

#endif  // SCENES_GROWBALL_GROW_BALL_CONFIG_H
