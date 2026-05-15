#ifndef SCENES_SNAKE_SNAKE_SCREEN_H
#define SCENES_SNAKE_SNAKE_SCREEN_H

#include "core/graphics/Geometry.h"
#include "core/runtime/GameScreen.h"
#include <cstdint>

namespace handheld {

// 贪吃蛇屏幕 — 经典 10×10 网格贪吃蛇游戏
class SnakeScreen : public GameScreen {
public:
    void enter(IPlatform& platform, IScreenHost& host) override;
    void update(IPlatform& platform, IScreenHost& host) override;
    void render(IPlatform& platform, IScreenHost& host) override;

private:
    static constexpr int16_t GRID_SIZE = 10;
    static constexpr int16_t CELL_SIZE = 8;
    static constexpr int16_t MAX_LENGTH = GRID_SIZE * GRID_SIZE;
    static constexpr uint32_t INITIAL_MOVE_INTERVAL = 8;
    static constexpr uint32_t MIN_MOVE_INTERVAL = 3;

    enum class Direction : uint8_t {
        UP,
        DOWN,
        LEFT,
        RIGHT,
    };

    struct Segment {
        int8_t x, y;
    };

    Direction _direction;
    Direction _next_direction;
    bool _game_over;
    bool _won;
    bool _paused = false;
    Segment _body[MAX_LENGTH];
    int16_t _body_length;
    int8_t _food_x, _food_y;
    int16_t _score;
    uint32_t _move_counter;
    uint32_t _move_interval;
    uint32_t _frame;
    uint32_t _rng_state;

    // 动画状态
    uint8_t _eat_effect_timer;
    int8_t _eat_effect_x, _eat_effect_y;
    uint8_t _shake_timer;
    int8_t _shake_offset_x, _shake_offset_y;

    void reset_game();
    void spawn_food();
    [[nodiscard]] bool is_occupied(int8_t x, int8_t y) const;
    uint32_t next_rng();
    void move_snake(IScreenHost& host);
};

}  // namespace handheld

#endif  // SCENES_SNAKE_SNAKE_SCREEN_H
