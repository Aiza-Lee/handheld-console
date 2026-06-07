#ifndef SCENES_PACMAN_PACMAN_SCREEN_H
#define SCENES_PACMAN_PACMAN_SCREEN_H

#include "core/graphics/Color.h"
#include "core/graphics/Geometry.h"
#include "core/runtime/GameScreen.h"
#include <cstdint>

namespace handheld {
namespace pacman {
namespace cfg {

// 颜色
constexpr Color WALL_COLOR = rgb565(15, 18, 60);
constexpr Color DOT_COLOR = rgb565(200, 170, 130);
constexpr Color POWER_COLOR = rgb565(220, 190, 150);
constexpr Color PAC_COLOR = rgb565(240, 210, 50);
constexpr Color BG_COLOR = rgb565(2, 2, 10);
constexpr Color GHOST1_COL = rgb565(220, 60, 60);
constexpr Color GHOST2_COL = rgb565(200, 80, 180);
constexpr Color VULN_COLOR = rgb565(50, 50, 170);
constexpr Color VULN_BLINK = rgb565(200, 200, 240);
constexpr Color AURA_COLOR = rgb565(180, 140, 220);
constexpr Color PARTICLE_COLOR = rgb565(240, 210, 80);
constexpr Color WIN_COLOR = rgb565(120, 200, 220);
constexpr Color MAZE_BORDER = rgb565(10, 12, 30);
constexpr Color HINT_COLOR = rgb565(120, 140, 120);
constexpr Color PAUSE_BG = rgb565(4, 6, 20);
constexpr Color PAUSE_TEXT = rgb565(220, 200, 100);

// 覆盖层尺寸
constexpr int16_t PAUSE_RECT_X = 10;
constexpr int16_t PAUSE_RECT_Y = 20;
constexpr int16_t PAUSE_RECT_W = 60;
constexpr int16_t PAUSE_RECT_H = 40;
constexpr int16_t END_RECT_X = 10;
constexpr int16_t END_RECT_Y = 20;
constexpr int16_t END_RECT_W = 60;
constexpr int16_t END_RECT_H = 48;
constexpr int16_t WIN_RECT_X = 10;
constexpr int16_t WIN_RECT_Y = 20;
constexpr int16_t WIN_RECT_W = 60;
constexpr int16_t WIN_RECT_H = 40;

// 布局
constexpr int16_t GRID = 10;
constexpr int16_t CELL = 8;

// 游戏参数 (20 FPS 基准)
constexpr uint32_t PAC_MOVE_INTERVAL = 6;
constexpr uint32_t GHOST_MOVE_INTERVAL = 8;
constexpr uint32_t POWER_DURATION = 120;
constexpr uint32_t DYING_TIMER = 20;
constexpr uint32_t CELEBRATION_DURATION = 30;
constexpr int16_t MAX_DEATH_PARTICLES = 10;
constexpr int8_t INIT_LIVES = 3;

// 分数
constexpr int16_t DOT_SCORE = 10;
constexpr int16_t GHOST_SCORE = 50;

// 初始位置
constexpr int8_t PAC_START_X = 5, PAC_START_Y = 8;
constexpr int8_t GHOST0_X = 4, GHOST0_Y = 4;
constexpr int8_t GHOST1_X = 5, GHOST1_Y = 4;

// 关卡配置
struct LevelConfig {
    const char* name;
    int ghost_interval;
    int power_duration;
    // 迷宫以 C-string 指针数组传递（10 行，每行 11 char 含 \0）
    const char* maze[GRID];
};

inline constexpr char MAZE_1[GRID][GRID + 1] = {
    "WWWWWWWWWW", "W........W", "W.WW.WW..W", "WPW...WP.W", "W...W....W",
    "W...W....W", "WPW...WP.W", "W.WW.WW..W", "W........W", "WWWWWWWWWW",
};

inline constexpr char MAZE_2[GRID][GRID + 1] = {
    "WWWWWWWWWW", "W...WW...W", "W.W....W.W", "WP.W..W.PW", "W..W..W..W",
    "W..W..W..W", "WP.W..W.PW", "W.W....W.W", "W...WW...W", "WWWWWWWWWW",
};

inline constexpr char MAZE_3[GRID][GRID + 1] = {
    "WWWWWWWWWW", "W........W", "W.W.WW.W.W", "WP......PW", "W..W..W..W",
    "W..W..W..W", "WP......PW", "W.W.WW.W.W", "W........W", "WWWWWWWWWW",
};

inline constexpr LevelConfig LEVELS[] = {
    {"LV1",
     8, 120,
     {(const char*)MAZE_1[0], (const char*)MAZE_1[1], (const char*)MAZE_1[2], (const char*)MAZE_1[3],
     (const char*)MAZE_1[4], (const char*)MAZE_1[5], (const char*)MAZE_1[6], (const char*)MAZE_1[7],
     (const char*)MAZE_1[8], (const char*)MAZE_1[9]}},
    {"LV2",
     6, 100,
     {(const char*)MAZE_2[0], (const char*)MAZE_2[1], (const char*)MAZE_2[2], (const char*)MAZE_2[3],
     (const char*)MAZE_2[4], (const char*)MAZE_2[5], (const char*)MAZE_2[6], (const char*)MAZE_2[7],
     (const char*)MAZE_2[8], (const char*)MAZE_2[9]}},
    {"LV3",
     5,  80,
     {(const char*)MAZE_3[0], (const char*)MAZE_3[1], (const char*)MAZE_3[2], (const char*)MAZE_3[3],
     (const char*)MAZE_3[4], (const char*)MAZE_3[5], (const char*)MAZE_3[6], (const char*)MAZE_3[7],
     (const char*)MAZE_3[8], (const char*)MAZE_3[9]}},
};
constexpr int LEVEL_COUNT = 3;

// 状态栏
constexpr int16_t STATUS_H = 7;
constexpr Color STATUS_BG = rgb565(4, 8, 18);

} // namespace cfg
} // namespace pacman
} // namespace handheld

namespace handheld {

// 吃豆人屏幕 — 经典 10×10 迷宫吃豆人游戏
class PacmanScreen : public GameScreen {
public:
    void enter(IPlatform& platform, IScreenHost& host) override;
    void update(IPlatform& platform, IScreenHost& host) override;
    void render(IPlatform& platform, IScreenHost& host) override;
    static void render_menu_preview(IDisplay& display, const Rect& box, uint32_t frame);

private:
    enum class Dir : uint8_t { UP, DOWN, LEFT, RIGHT, NONE };
    enum class State : uint8_t { PLAYING, DYING, GAME_OVER };
    enum class GhostState : uint8_t { CHASE, VULNERABLE, RETURNING };

    struct Ghost {
        int8_t x, y;
        Dir dir;
        GhostState state;
    };

    Dir _pac_dir;
    Dir _pac_next;
    int8_t _pac_x, _pac_y;
    State _state;
    bool _paused = false;
    Ghost _ghosts[2];
    bool _dots[pacman::cfg::GRID][pacman::cfg::GRID];
    bool _power[pacman::cfg::GRID][pacman::cfg::GRID];
    int8_t _dot_count;
    int16_t _score;
    int8_t _lives;
    int8_t _level = 0;
    uint32_t _power_timer;
    uint32_t _pac_move_counter;
    uint32_t _ghost_move_counter;
    uint32_t _dying_timer;
    uint32_t _frame;
    uint32_t _rng;

    // 动画状态
    struct DeathParticle {
        int8_t x, y;
        int8_t vx, vy;
        uint8_t life;
    };
    DeathParticle _death_particles[pacman::cfg::MAX_DEATH_PARTICLES];
    uint8_t _death_particle_count;
    uint8_t _celebration_timer;

    void reset_game();
    void load_level(uint8_t level);
    [[nodiscard]] bool is_wall(int8_t x, int8_t y) const;
    [[nodiscard]] static bool is_valid_cell(int8_t x, int8_t y);
    [[nodiscard]] static int8_t manhattan(int8_t x1, int8_t y1, int8_t x2, int8_t y2);
    bool try_move(int8_t x, int8_t y, Dir dir, int8_t& nx, int8_t& ny) const;
    [[nodiscard]] int8_t ghost_target_x(const Ghost& g) const;
    [[nodiscard]] int8_t ghost_target_y(const Ghost& g) const;
    void move_pacman(IScreenHost& host);
    void move_ghosts();
    void check_ghost_collision(IScreenHost& host);
    void die(IScreenHost& host);
    void update_particles();
    uint32_t next_rng();
};

} // namespace handheld

#endif // SCENES_PACMAN_PACMAN_SCREEN_H
