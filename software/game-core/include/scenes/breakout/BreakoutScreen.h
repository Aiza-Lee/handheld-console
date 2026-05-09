#ifndef SCENES_BREAKOUT_BREAKOUT_SCREEN_H
#define SCENES_BREAKOUT_BREAKOUT_SCREEN_H

#include "core/graphics/Geometry.h"
#include "core/runtime/GameScreen.h"
#include <cstdint>

namespace handheld {

// 弹砖块屏幕 — 经典打砖块游戏
class BreakoutScreen : public GameScreen {
public:
    void enter(IPlatform& platform, IScreenHost& host) override;
    void update(IPlatform& platform, IScreenHost& host) override;
    void render(IPlatform& platform, IScreenHost& host) override;

private:
    static constexpr int16_t PADDLE_W = 16;
    static constexpr int16_t PADDLE_H = 4;
    static constexpr int16_t PADDLE_Y = 74;
    static constexpr int16_t BALL_W = 2;
    static constexpr int16_t BALL_H = 2;
    static constexpr int16_t BRICK_ROWS = 5;
    static constexpr int16_t BRICKS_PER_ROW = 8;
    static constexpr int16_t BRICK_W = 8;
    static constexpr int16_t BRICK_H = 5;
    static constexpr int16_t BRICK_GAP_X = 2;
    static constexpr int16_t BRICK_GAP_Y = 1;
    static constexpr int16_t BRICK_OY = 6;

    enum class State : uint8_t { ATTACHED, ACTIVE, DYING, GAME_OVER };

    State _state;
    int16_t _paddle_x;
    int16_t _ball_x, _ball_y;
    int8_t _ball_vx, _ball_vy;
    uint8_t _bricks[BRICK_ROWS];
    int16_t _bricks_remaining;
    int16_t _score;
    int8_t _lives;
    uint32_t _dying_timer;
    uint32_t _frame;
    uint32_t _rng;

    void reset_game();
    void launch_ball();
    void check_paddle_collision();
    bool check_brick_collision();
    void lose_life();
    [[nodiscard]] Rect brick_rect(int16_t row, int16_t col) const;
    uint32_t next_rng();
};

}  // namespace handheld

#endif  // SCENES_BREAKOUT_BREAKOUT_SCREEN_H
