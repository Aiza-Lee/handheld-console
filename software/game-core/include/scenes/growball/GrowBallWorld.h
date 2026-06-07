#ifndef SCENES_GROWBALL_GROW_BALL_WORLD_H
#define SCENES_GROWBALL_GROW_BALL_WORLD_H

#include "core/graphics/Color.h"
#include <cstdint>

namespace handheld::growball::cfg {

// ── 显示（与硬件 80×80 匹配）─────────────────────────────────────
constexpr int16_t SCREEN_WIDTH = 80;
constexpr int16_t SCREEN_HEIGHT = 80;
constexpr int16_t CAMERA_OFFSET_X = SCREEN_WIDTH / 2;  // 世界→屏幕映射的 X 偏移
constexpr int16_t CAMERA_OFFSET_Y = SCREEN_HEIGHT / 2; // 世界→屏幕映射的 Y 偏移

// ── 世界 ─────────────────────────────────────────────────────────
constexpr int16_t WORLD_W = 200;
constexpr int16_t WORLD_H = 200;

// ── 网格 ─────────────────────────────────────────────────────────
constexpr int16_t GRID_SPACING = 20; // 网格线在世界空间中的间距

// ── 玩家 ─────────────────────────────────────────────────────────
constexpr float PLAYER_INIT_RADIUS = 8.0F;
constexpr float PLAYER_MAX_RADIUS = 100.0F;
constexpr int TARGET_SCORE = static_cast<int>(PLAYER_MAX_RADIUS * 10);
constexpr float PLAYER_THRUST = 0.5F;
constexpr float PLAYER_THRUST_MULT = 0.5F; // 施加到速度上限的推力比例
constexpr float PLAYER_FRICTION = 0.93F;
constexpr float PLAYER_MAX_SPEED = 3.2F;
constexpr Color PLAYER_COLOR = rgb565(80, 180, 240);

// ── 食物 ─────────────────────────────────────────────────────────
constexpr int16_t FOOD_COUNT = 20;
constexpr float FOOD_MIN_RADIUS = 1.0F;
constexpr float FOOD_MAX_RADIUS = 3.5F;
constexpr float FOOD_MASS = 3.0F;
constexpr Color FOOD_COLOR = rgb565(220, 200, 100);

// ── AI ───────────────────────────────────────────────────────────
constexpr int16_t AI_COUNT = 4;
constexpr float AI_MIN_RADIUS = 6.0F;
constexpr float AI_MAX_RADIUS = 18.0F;
constexpr float AI_THRUST = 0.18F;
constexpr float AI_FRICTION = 0.94F;
constexpr float AI_MAX_SPEED = 1.5F;
constexpr float AI_WANDER_CHANGE_MS = 2500.0F;
constexpr float AI_DETECT_RANGE = 90.0F;       // 检测其他球的感知范围
constexpr float AI_CHASE_RANGE = 60.0F;        // 追击更小目标的范围
constexpr float AI_FLEE_RANGE = 60.0F;         // 逃离更大目标的范围
constexpr float AI_EAT_RATIO = 0.4F;           // AI 互食的最小尺寸比
constexpr float AI_SPAWN_MIN_DIST_MULT = 5.0F; // 生成时与玩家的最小距离（玩家半径的倍数）
constexpr int AI_SPAWN_RETRIES = 10;           // 生成位置的重试次数
constexpr float AI_WANDER_TARGET_DIST = 30.0F; // 漫游目标距离
constexpr float AI_DETECT_SIZE_RATIO = 1.1F;   // 检测时的尺寸比阈值
constexpr float AI_FOOD_GROWTH = 0.7F;         // AI 吃食物的成长倍率
constexpr float AI_VS_AI_GROWTH = 0.3F;        // AI 互食的成长倍率

// ── AI 颜色 ─────────────────────────────────────────────────────
constexpr Color AI_COLORS[] = {
    rgb565(240, 100, 80), rgb565(100, 220, 100), rgb565(220, 140, 60), rgb565(180, 100, 220), rgb565(240, 200, 80),
};

// ── 碰撞 ─────────────────────────────────────────────────────────
constexpr float PLAYER_VS_AI_OVERLAP = 0.92F;     // 玩家与 AI 的碰撞重叠系数
constexpr float PLAYER_EAT_AI_SIZE_RATIO = 1.1F;  // 玩家吃 AI 需大于此比例
constexpr float AI_EAT_PLAYER_SIZE_RATIO = 1.1F;  // AI 吃玩家需大于此比例
constexpr float PLAYER_EAT_AI_GROWTH = 0.4F;      // 玩家吃 AI 的成长倍率
constexpr float PLAYER_EAT_FOOD_MIN_RATIO = 0.5F; // 玩家可吃食物的最小尺寸比

// ── 摄像机 ───────────────────────────────────────────────────────
constexpr float ZOOM_MIN = 0.35F;
constexpr float ZOOM_MAX = 1.0F;
constexpr float ZOOM_BASE = 80.0F;   // 缩放基准
constexpr float ZOOM_DIVISOR = 5.0F; // 缩放除数（玩家半径的倍率）

// ── 生长音效阈值 ─────────────────────────────────────────────────
constexpr float GROWTH_SFX_BIG_THRESHOLD = 2.0F;    // 超过此半径增长播放"吃敌人"音效
constexpr float GROWTH_SFX_SMALL_THRESHOLD = 0.01F; // 超过此半径增长播放"吃食物"音效

// ── 背景 ─────────────────────────────────────────────────────────
constexpr Color BG_COLOR = rgb565(8, 12, 28);
constexpr Color GRID_COLOR = rgb565(14, 20, 42);
constexpr Color BORDER_COLOR = rgb565(60, 60, 100);

// ── 暂停 ─────────────────────────────────────────────────────────
constexpr int16_t PAUSE_RECT_X = 10;
constexpr int16_t PAUSE_RECT_Y = 20;
constexpr int16_t PAUSE_RECT_W = 60;
constexpr int16_t PAUSE_RECT_H = 40;
constexpr Color PAUSE_BG = rgb565(5, 8, 18);
constexpr Color PAUSE_TEXT = rgb565(180, 200, 255);

// ── 胜利 ─────────────────────────────────────────────────────────
constexpr int16_t WIN_RECT_X = 10;
constexpr int16_t WIN_RECT_Y = 20;
constexpr int16_t WIN_RECT_W = 60;
constexpr int16_t WIN_RECT_H = 48;
constexpr Color VICTORY_BG = rgb565(0, 10, 5);
constexpr Color VICTORY_TEXT = rgb565(80, 240, 140);

// ── 游戏结束 ─────────────────────────────────────────────────────
constexpr int16_t END_RECT_X = 10;
constexpr int16_t END_RECT_Y = 20;
constexpr int16_t END_RECT_W = 60;
constexpr int16_t END_RECT_H = 48;
constexpr Color GAME_OVER_BG = rgb565(0, 0, 0);
constexpr Color GAME_OVER_TEXT = rgb565(240, 80, 80);

} // namespace handheld::growball::cfg

namespace handheld {

class GrowBallWorld {
public:
    struct Ball {
        float x, y, vx, vy, radius;
        Color color;
    };
    struct Food {
        float x, y, radius;
        Color color;
    };
    struct AiBall {
        float x, y, vx, vy, radius;
        Color color;
        float wander_angle;
        uint32_t last_change_ms;
    };

    void init(uint32_t now_ms);

    Ball& player() { return _player; }
    const Ball& player() const { return _player; }
    Food* foods() { return _foods; }
    const Food* foods() const { return _foods; }
    AiBall* ai() { return _ai; }
    const AiBall* ai() const { return _ai; }

    // 执行一帧模拟。victory/game_over 为输出标志。
    void simulate(float input_vx, float input_vy, uint32_t now_ms, bool& victory, bool& game_over);

    static float speed_limit(float radius);
    static void clamp_to_world(Ball& ball);

private:
    void spawn_food(int index);
    void spawn_ai(int index, uint32_t now_ms);
    void update_ai(uint32_t now_ms);
    void resolve_collisions(uint32_t now_ms, bool& victory, bool& game_over);

    Ball _player;
    Food _foods[growball::cfg::FOOD_COUNT];
    AiBall _ai[growball::cfg::AI_COUNT];
};

} // namespace handheld

#endif // SCENES_GROWBALL_GROW_BALL_WORLD_H
