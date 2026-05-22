#ifndef SCENES_BREAKOUT_BREAKOUT_SCREEN_H
#define SCENES_BREAKOUT_BREAKOUT_SCREEN_H

#include "core/effects/ParticleSystem.h"
#include "core/graphics/Geometry.h"
#include "core/runtime/GameScreen.h"
#include <cstdint>

namespace handheld {

class BreakoutScreen : public GameScreen {
public:
    void enter(IPlatform& platform, IScreenHost& host) override;
    void update(IPlatform& platform, IScreenHost& host) override;
    void render(IPlatform& platform, IScreenHost& host) override;

private:
    static constexpr int16_t BRICK_COLS = 15;
    static constexpr int16_t MAX_BRICK_ROWS = 11;
    static constexpr int16_t TILE_W = 75;
    static constexpr int16_t TILE_H = 55;

    enum class State : uint8_t { ATTACHED, ACTIVE, GAME_OVER };

    struct Ball {
        int8_t x, y;
        int8_t vx, vy;
        int8_t px, py;
        bool active;
    };

    struct PowerUp {
        int8_t x, y;
        uint8_t type;
        bool active;
    };

    // ── 游戏状态 ──
    State _state;
    bool _paused = false;
    int16_t _paddle_x;
    int16_t _score;
    uint8_t _level;
    uint32_t _frame;
    uint32_t _rng;

    // ── 瓦片缓存（仅砖块区域 75×55 = 4125 字节）──
    uint8_t _tile[TILE_H][TILE_W];

    // ── 砖块网格 ──
    uint8_t _bricks[MAX_BRICK_ROWS][BRICK_COLS];
    int16_t _bricks_remaining;

    // ── 多球 ──
    Ball _balls[16];
    int16_t _ball_count;

    // ── 粒子（复用组件，上限 12）──
    ParticleSystem<12> _particles;

    // ── 道具 ──
    PowerUp _powerups[3];

    // ── 庆祝 ──
    uint8_t _celebration_timer;

    // ── 方法 ──
    void reset_game();
    void load_level(uint8_t level);
    void build_tile_cache();
    [[nodiscard]] bool tile_at(int8_t x, int8_t y) const;
    void launch_ball();
    void spawn_triple_balls(int8_t x, int8_t y);
    void split_balls();
    void move_balls(IScreenHost& host);
    void move_powerups();
    void check_paddle_collisions(IScreenHost& host);
    bool check_tile_collision(int8_t test_x, int8_t test_y, Ball& ball, IScreenHost& host);
    void destroy_brick(int16_t row, int16_t col, uint8_t brick_type, IScreenHost& host);
    void spawn_brick_particles(int16_t row, int16_t col, uint8_t brick_type);
    void check_powerup_collection();
    void try_spawn_powerup(int16_t row, int16_t col);
    uint32_t next_rng();
};

}  // namespace handheld

#endif
