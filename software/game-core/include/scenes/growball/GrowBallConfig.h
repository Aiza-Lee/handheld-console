#ifndef SCENES_GROWBALL_GROW_BALL_CONFIG_H
#define SCENES_GROWBALL_GROW_BALL_CONFIG_H

#include "core/graphics/Color.h"
#include <cstdint>

namespace handheld::growball::cfg {

// 音频
constexpr bool ENABLE_BGM = false;

// 世界
constexpr int16_t WORLD_W = 200;
constexpr int16_t WORLD_H = 200;

// 玩家
constexpr float PLAYER_INIT_RADIUS = 4.0F;
constexpr float PLAYER_MAX_RADIUS = 100.0F;
constexpr int   TARGET_SCORE = static_cast<int>(PLAYER_MAX_RADIUS * 10);
constexpr float PLAYER_THRUST = 0.5F;
constexpr float PLAYER_FRICTION = 0.93F;
constexpr float PLAYER_MAX_SPEED = 3.2F;
constexpr Color PLAYER_COLOR = rgb565(80, 180, 240);

// 食物
constexpr int16_t FOOD_COUNT = 20;
constexpr float FOOD_MIN_RADIUS = 1.0F;
constexpr float FOOD_MAX_RADIUS = 2.5F;
constexpr float FOOD_MASS = 3.0F;
constexpr Color FOOD_COLOR = rgb565(220, 200, 100);

// AI
constexpr int16_t AI_COUNT = 4;
constexpr float AI_MIN_RADIUS = 6.0F;
constexpr float AI_MAX_RADIUS = 18.0F;
constexpr float AI_THRUST = 0.18F;
constexpr float AI_FRICTION = 0.94F;
constexpr float AI_MAX_SPEED = 1.5F;
constexpr float AI_WANDER_CHANGE_MS = 2500.0F;
constexpr float AI_DETECT_RANGE = 90.0F;
constexpr float AI_CHASE_RANGE = 75.0F;
constexpr float AI_FLEE_RANGE = 60.0F;
constexpr float AI_EAT_RATIO = 0.4F;
constexpr float AI_SPAWN_MIN_DIST_MULT = 5.0F;

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
constexpr Color BORDER_COLOR = rgb565(60, 60, 100);

// 暂停
constexpr Color PAUSE_BG = rgb565(5, 8, 18);
constexpr Color PAUSE_TEXT = rgb565(180, 200, 255);

// 胜利
constexpr Color VICTORY_BG = rgb565(0, 10, 5);
constexpr Color VICTORY_TEXT = rgb565(80, 240, 140);

// 游戏结束
constexpr Color GAME_OVER_BG = rgb565(0, 0, 0);
constexpr Color GAME_OVER_TEXT = rgb565(240, 80, 80);

}  // namespace handheld::growball::cfg

#endif  // SCENES_GROWBALL_GROW_BALL_CONFIG_H
