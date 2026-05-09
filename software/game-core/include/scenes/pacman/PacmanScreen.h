#ifndef SCENES_PACMAN_PACMAN_SCREEN_H
#define SCENES_PACMAN_PACMAN_SCREEN_H

#include "core/graphics/Geometry.h"
#include "core/runtime/GameScreen.h"
#include <cstdint>

namespace handheld {

// 吃豆人屏幕 — 经典 10×10 迷宫吃豆人游戏
class PacmanScreen : public GameScreen {
public:
    void enter(IPlatform& platform, IScreenHost& host) override;
    void update(IPlatform& platform, IScreenHost& host) override;
    void render(IPlatform& platform, IScreenHost& host) override;

private:
    static constexpr int16_t GRID = 10;
    static constexpr int16_t CELL = 8;
    static constexpr uint32_t PAC_MOVE_INTERVAL = 6;
    static constexpr uint32_t GHOST_MOVE_INTERVAL = 8;
    static constexpr uint32_t POWER_DURATION = 120;

    enum class Dir : uint8_t { UP, DOWN, LEFT, RIGHT, NONE };
    enum class State : uint8_t { PLAYING, DYING, GAME_OVER };
    enum class GhostState : uint8_t { CHASE, VULNERABLE, RETURNING };

    struct Ghost {
        int8_t x, y;
        Dir dir;
        GhostState state;
    };

    static constexpr char _MAZE[GRID][GRID] = {
        {'W','W','W','W','W','W','W','W','W','W'},
        {'W','.','.','.','.','.','.','.','.','W'},
        {'W','.','W','W','.','W','W','.','.','W'},
        {'W','P','W','.','.','.','W','P','.','W'},
        {'W','.','.','.','W','.','.','.','.','W'},
        {'W','.','.','.','W','.','.','.','.','W'},
        {'W','P','W','.','.','.','W','P','.','W'},
        {'W','.','W','W','.','W','W','.','.','W'},
        {'W','.','.','.','.','.','.','.','.','W'},
        {'W','W','W','W','W','W','W','W','W','W'},
    };

    Dir _pac_dir;
    Dir _pac_next;
    int8_t _pac_x, _pac_y;
    State _state;
    Ghost _ghosts[2];
    bool _dots[GRID][GRID];
    bool _power[GRID][GRID];
    int8_t _dot_count;
    int16_t _score;
    int8_t _lives;
    uint32_t _power_timer;
    uint32_t _pac_move_counter;
    uint32_t _ghost_move_counter;
    uint32_t _dying_timer;
    uint32_t _frame;
    uint32_t _rng;

    void reset_game();
    [[nodiscard]] bool is_wall(int8_t x, int8_t y) const;
    [[nodiscard]] static bool is_valid_cell(int8_t x, int8_t y);
    [[nodiscard]] static int8_t manhattan(int8_t x1, int8_t y1, int8_t x2, int8_t y2);
    bool try_move(int8_t x, int8_t y, Dir dir, int8_t& nx, int8_t& ny) const;
    [[nodiscard]] int8_t ghost_target_x(const Ghost& g) const;
    [[nodiscard]] int8_t ghost_target_y(const Ghost& g) const;
    void move_pacman();
    void move_ghosts();
    void check_ghost_collision();
    void die();
    uint32_t next_rng();
};

}  // namespace handheld

#endif  // SCENES_PACMAN_PACMAN_SCREEN_H
