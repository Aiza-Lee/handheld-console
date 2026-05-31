#ifndef SCENES_BREAKOUT_BREAKOUT_SCREEN_H
#define SCENES_BREAKOUT_BREAKOUT_SCREEN_H

#include "core/effects/ParticleSystem.h"
#include "core/graphics/Geometry.h"
#include "core/runtime/GameScreen.h"
#include "scenes/breakout/BrickGrid.h"
#include <cstdint>

namespace handheld {

class BreakoutScreen : public GameScreen {
public:
    void enter(IPlatform& platform, IScreenHost& host) override;
    void update(IPlatform& platform, IScreenHost& host) override;
    void render(IPlatform& platform, IScreenHost& host) override;

private:
    enum class State : uint8_t { ATTACHED, ACTIVE, GAME_OVER };

    struct Ball {
        int8_t x, y;
        int8_t vx, vy;
        int8_t px, py;   // previous position (for trail)
        bool active;
    };

    struct PowerUp {
        int8_t x, y;
        uint8_t type;
        bool active;
    };

    static constexpr int16_t MAX_BALLS = 16;
    static constexpr int16_t MAX_POWERUPS = 3;

    // ── 游戏状态 ──
    breakout::BrickGrid _grid;
    State _state = State::ATTACHED;
    bool _paused = false;
    int16_t _paddle_x = 32;
    int16_t _score = 0;
    uint8_t _level = 0;
    uint32_t _frame = 0;
    uint32_t _rng = 12345;
    uint8_t _celebration = 0;       // celebration timer

    // ── 实体 ──
    Ball _balls[MAX_BALLS]{};
    int16_t _ball_count = 0;
    PowerUp _powerups[MAX_POWERUPS]{};
    ParticleSystem<12> _particles;

    // ── 内部方法 ──
    void reset_game();
    void load_level(uint8_t level);
    uint32_t next_rng();

    // 球
    void launch_ball();
    void spawn_triple(int8_t x, int8_t y);
    void split_balls();
    void move_balls(IScreenHost& host);
    // 单步碰撞：返回 true 表示球已销毁（掉出底部）
    bool step_ball(Ball& b, IScreenHost& host);

    // 道具
    void spawn_powerup(int16_t row, int16_t col);
    void move_powerups();
    void collect_powerups();

    // 粒子
    void spawn_particles(int16_t row, int16_t col, uint8_t brick_type);

    // 渲染
    void draw_status_bar(IDisplay& display) const;
    void draw_bricks(IDisplay& display) const;
    void draw_overlay(IDisplay& display) const;
};

}  // namespace handheld

#endif
