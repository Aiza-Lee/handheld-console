#ifndef SCENES_INVADERS_INVADERS_SCREEN_H
#define SCENES_INVADERS_INVADERS_SCREEN_H

#include "core/graphics/Geometry.h"
#include "core/runtime/GameScreen.h"
#include <cstdint>

namespace handheld {

// 太空侵略者屏幕 — 经典射击游戏
class InvadersScreen : public GameScreen {
public:
    void enter(IPlatform& platform, IScreenHost& host) override;
    void update(IPlatform& platform, IScreenHost& host) override;
    void render(IPlatform& platform, IScreenHost& host) override;

private:
    static constexpr int16_t GRID_COLS = 6;
    static constexpr int16_t GRID_ROWS = 4;
    static constexpr int16_t ENEMY_W = 6;
    static constexpr int16_t ENEMY_H = 5;
    static constexpr int16_t CELL_W = 7;
    static constexpr int16_t CELL_H = 8;
    static constexpr int16_t GRID_OX = 19;
    static constexpr int16_t GRID_OY = 12;
    static constexpr int16_t PLAYER_W = 7;
    static constexpr int16_t PLAYER_H = 5;
    static constexpr int16_t PLAYER_Y = 72;
    static constexpr int16_t STATUS_H = 8;
    static constexpr int16_t MAX_PARTICLES = 12;
    static constexpr int16_t MAX_EBULLETS = 2;

    enum class State : uint8_t { PLAYING, DYING, GAME_OVER };

    struct EnemyBullet {
        int16_t x, y;
        bool active;
    };

    struct Particle {
        int8_t x, y;
        int8_t vx, vy;
        uint8_t life;
    };

    // 敌人状态
    bool _enemy_alive[GRID_COLS * GRID_ROWS];
    int16_t _enemy_base_x;
    int16_t _enemy_base_y;
    int8_t _enemy_dir_x;
    int16_t _enemies_alive_count;

    // 玩家
    int16_t _player_x;
    State _state;
    bool _paused = false;

    // 子弹
    int16_t _bullet_x, _bullet_y;
    bool _bullet_active;
    EnemyBullet _ebullets[MAX_EBULLETS];

    // 爆炸粒子
    Particle _particles[MAX_PARTICLES];
    int16_t _particle_count;

    // 分数
    int16_t _score;
    int8_t _lives;
    int16_t _high_score;

    // 计时
    uint32_t _frame;
    uint32_t _move_timer;
    uint32_t _move_interval;
    uint32_t _shoot_timer;
    uint32_t _dying_timer;
    uint32_t _rng;

    void reset_game();
    void respawn_player();
    void player_shoot(IScreenHost& host);
    void enemy_shoot();
    void move_enemies();
    void move_bullets(IScreenHost& host);
    void spawn_particles(int16_t x, int16_t y);
    void update_particles();
    void draw_enemy(IDisplay& display, int16_t px, int16_t py, int16_t row, uint32_t frame) const;
    void draw_player(IDisplay& display, int16_t x, uint32_t frame) const;
    uint32_t next_rng();
};

}  // namespace handheld

#endif  // SCENES_INVADERS_INVADERS_SCREEN_H
